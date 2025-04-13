#include <chrono>
#include <ctime>
#include <functional>
#include <system_error>
#include <thread>
#include <stop_token>
#include <cassert>
#include <memory>
#include <iostream>
#include <mutex>


#ifndef CHESSTIMER_H
#define CHESSTIMER_H
namespace chessTimer {
    class Timer {
        inline static bool s_started = false;
        inline static bool s_mutexLocked = false;
        double m_maxDuration;
        std::jthread m_timingThread;
        std::mutex &m_startMutex;

        public:
        inline static bool s_finished = false;
            Timer(std::mutex &mutex, double maxDuration = 5.0) :
            m_maxDuration(maxDuration),
                m_startMutex(mutex)
            {
                threadInit();
            }

            // Timer(const Timer &other) {
            // }

            void start() {
                Timer::s_started = true;
            }

            bool awaitFinish() {
                while (!Timer::s_finished) {
                    continue;
                }
                return Timer::s_finished;
            }

            void reset() {
                Timer::s_started = false;
                Timer::s_finished = false;
                Timer::s_mutexLocked = false;
                threadInit();
            }

        private:
            void threadInit() {
                auto f = std::bind_front(&Timer::childInit, this);
                m_timingThread = std::jthread(f);
                // ensure that the child thread has locked it successfully
                // sometimes after exiting this the thread pool threads are able to start execution without the 

                using namespace std::chrono_literals;
                std::chrono::milliseconds time = 10ms;

                // block while the mutex hasnt locked
                // this is because sometimes the threadPool can create children and start them before the mutex has been locked
                // its weird ik and probably prone to errors but its the best I can do
                // might be an idea to add a system to exit with an error code if unable to lock the mutex after some time
                // kind of a way to allow bad pcs to not shut down entirely if struggling to init the child
                // the alternative because this keeps the thread awake is to sleep the thread or introduce another mutex
                // The problem with sleeping is that it could be a lot of time for an uneccessary wait
                // The problem with introducing another mutex is the risk of deadlocking if poor logic
                while (!Timer::s_mutexLocked) {
                    continue;
                }

            }
            // in the timing thread
            void childInit() {
                // locks the mutex
                std::unique_lock<std::mutex> lock(m_startMutex, std::defer_lock);
                try {
                    lock.lock();
                } catch (const std::system_error &e) {
                    std::cout << "System error: " << e.code() << "\n" << e.what();
                    assert(false);
                }
                // cant use try_lock because threadInit is also attempting to lock it as it must block 
                Timer::s_mutexLocked = true;
                // block while we havent started

                while (!Timer::s_started) {
                    continue;
                }
                // unlock the mutex to start the work
                lock.unlock();
                count();
                Timer::s_finished = true;
            }

            void count() {
                std::cout << "timer starting\n";
                const std::time_t startTime = std::time(nullptr);
                std::time_t currentTime = std::time(nullptr);
                double difference = difftime(currentTime, startTime);
                while (difference < m_maxDuration) {
                    currentTime = std::time(nullptr);
                    difference = difftime(currentTime, startTime);
                    // std::cout << difference << "\n";
                }
                std::cout << "timer ending\n";
            }
    };

}
#endif
