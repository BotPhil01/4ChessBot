#include "bitboard.h"
#include "board.h"
#include"engine.h"
#include "move.h"
#include "timer.h"
#include <chrono>
#include <cstdint>
#include <functional>
#include <thread>
#include <condition_variable>

#ifndef THREADING_H
#define THREADING_H

// used for spawning multiple threads off 
namespace threading {
    using namespace std::chrono_literals;
    typedef struct ThreadData {
        std::reference_wrapper<board::Board> board;
        types::PieceColour colour;
        std::reference_wrapper<bool> timerSignal;
    } ThreadData;

    // class EngineThread {
    //     // spawns a thread 
    //     engine::Engine engine;
    //     std::jthread thread;
    //     public:
    //     EngineThread(ThreadData p_data) : 
    //     engine(p_data.board.get(), p_data.colour) 
    //     {}

    //     void start(std::reference_wrapper<move::Move> out) {
    //         auto func = std::bind_front(&EngineThread::foo, this, out);
    //         thread = std::jthread(func);
    //     }

    //     private:
    //     void foo(std::reference_wrapper<move::Move> out) {
    //         out.get() = engine.chooseNextMove();
    //     }
    // };

    // this object will be copied anyway 
    // copying the reference_wrapper object is an actual reference
    // child must take the correct mutex
    // a copy of the board

    // child class for evaluating a singular move
    class ChildThread {
        // spawns a thread 
        engine::Engine m_engine;
        std::reference_wrapper<std::mutex> m_workMutex;
        int m_workIndex;
        std::reference_wrapper<std::vector<std::int_fast16_t>> m_resultVector;
        std::jthread m_thread;
        public:
        ChildThread(
                board::Board p_board,
                types::PieceColour p_colour,
                std::reference_wrapper<std::mutex> p_workMutex,
                int p_workIndex,
                std::reference_wrapper<std::vector<std::int_fast16_t>> p_resultVector,
                std::reference_wrapper<bool> m_finished
                ) : 
            m_engine(p_board, p_colour, m_finished),
            m_workMutex(p_workMutex),
            m_workIndex(p_workIndex),
            m_resultVector(p_resultVector)
        {
            start();
        }

        void start() {
            const auto func = std::bind_front(&ChildThread::eval, this);
            m_thread = std::jthread(func);
        }

        void join() {
            if (m_thread.joinable()) {
                m_thread.join();
            }
        }

        private:
        // the thread function
        void eval() {
            waitToStart();
            if (!chessTimer::Timer::s_terminating) {
                m_resultVector.get()[m_workIndex] = m_engine.evalIndex(m_workIndex);
            }
        }

        // waits for the mutex to be unlocked by the timer
        void waitToStart() {
            std::cout << "waiting to start\n";
            std::lock_guard<std::mutex> lock(m_workMutex.get());
            std::cout << "waiting finished\n";
        }
    };

    namespace {
        class EngineThreadPool {
            std::vector<ChildThread> m_threads;
            std::vector<std::int_fast16_t> m_results;
            std::mutex m_workMutex;
            chessTimer::Timer m_timer;
            std::reference_wrapper<board::Board> m_board;
            types::PieceColour m_colour;

            public:
            EngineThreadPool(std::reference_wrapper<board::Board> p_board, types::PieceColour p_colour) :
                m_workMutex(),
                m_timer(m_workMutex, 6),
                m_board(p_board),
                m_colour(p_colour)
            {
                init();
            }

            // causes seg fault 
            void init() {
                m_results.clear();
                m_threads.clear();
                // add work to workstack
                // const int movesSize = m_board.get().generateLegalMoves(m_colour).size();
                const int movesSize = 3; //DEBUG
                m_results.reserve(movesSize);
                m_threads.reserve(movesSize);

                // create the m_threads
                for (int i = 0; i < movesSize; i++) {
                    m_threads.emplace_back(
                            m_board,
                            m_colour,
                            std::ref(m_workMutex),
                            i,
                            std::ref(m_results),
                            std::ref(m_timer.s_finished)
                            );
                    m_results.emplace_back(INT_FAST16_MIN);
                }
            }

            const std::vector<std::int_fast16_t> getResults() {
                return m_results;
            }

            void start() {
                std::cout << "starting thread pool\n";
                m_timer.start();
            }
            void waitForFinish() {
                while (!m_timer.s_finished) {
                    continue;
                }
                joinThreads();
            }

            void reset() {
                m_timer.reset();
                init();
            }

            void terminate() {
                m_timer.terminate();
            }

            private:
            void joinThreads() {
                for (int i = 0; i < m_threads.size(); i++) {
                    m_threads[i].join();
                }
                std::cout << "threadpool threads finished\n";
            }
        };
    }

    // class responsible for evaluating a singular position
    // uses thread pool
    class MasterThread {
        EngineThreadPool m_threadPool;
        std::reference_wrapper<board::Board> m_board;
        const types::PieceColour m_colour;
        public:
        MasterThread(std::reference_wrapper<board::Board> p_board, const types::PieceColour p_colour) : 
            m_threadPool(p_board, p_colour),
            m_board(p_board),
            m_colour(p_colour)
        {}

        // void printSequentially() {
        //     std::reference_wrapper<bool> signal = m_timer.getFinishedSignal();
        //     int i = 0;
        //     while (!signal.get()) {
        //         std::this_thread::sleep_for(3000ms);
        //         std::cout << i++ << "\n";
        //         m_data.board.get().print();
        //     }
        // }
        move::Move chooseNextMove() {
            m_threadPool.start();
            m_threadPool.waitForFinish();
            // printSequentially();
            int index = 0;
            std::int_fast16_t bestEval = INT_FAST16_MIN;
            std::vector<std::int_fast16_t> results = m_threadPool.getResults();

            // only want to reset after relevant data (results) has been read
            m_threadPool.reset();

            // find the best result
            for (int i = 0; i < results.size(); i++) {
                if (results[i] > bestEval) {
                    bestEval = results[i];
                    index = i;
                }
            }
            board::Board &board = m_board.get();
            const std::vector<move::Move> moves = board.generateLegalMoves(m_colour);
            assert(moves.size() > 0);
            assert(index > -1);
            return moves[index];
        }

        void terminate() {
            m_threadPool.terminate();
        }
    };

}
#endif
