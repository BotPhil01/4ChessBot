#include<iostream>
#include<vector>
#include<algorithm>
#include<exception>
#include<numeric>
#include<cstdint>
#include<ctime>
#include<fstream>
#include<thread>
#include<stop_token>
#include<mutex>

#include"board.h"
#include"helper.h"
#include"evaluator.h"
#include"transpo.h"

#ifndef ENGINE_H
#define ENGINE_h

namespace engine {
    
    static thread_local std::mutex workMutex;

    // TODO PROBLEM THIS FUNCTION REQUIRES NOT ONLY ENGINE BUT ALSO ENGINE::ENGINECHILD
    // ITS POSSIBLE TO FORWARD DECLARE ENGINE BUT NOT THE NESTED ENGINECHILD
    // A FEW OPTIONS:
    // PASS THE FUNCTION AS PARAM TO ENGINE -> LOOKS MESSY
    // UNNEST THE CLASS -> PUT IN PRIVATE NAMESPACE MAYBE?
    // MOST LIKELY WILL UNNEST AND PUT BOTH FUNCTION AND CHILD IN BLANK NAMESPACE AND FORWARD DECLARE JUST ENGINE 
    // ITS 2:20AM IM GOING TO SLEEP 
    // FUCK THIS
    class Engine;
    void startChild(
        Engine *parent, 
        std::reference_wrapper<std::vector<std::shared_ptr<types::Move>>> moves, 
        std::reference_wrapper<std::vector<std::int_fast16_t>> evals, 
        std::reference_wrapper<std::stack<unsigned int, std::list<unsigned int>>> workStack,
        std::reference_wrapper<std::mutex> initMutex,
        std::reference_wrapper<std::stop_source> timeExceeded
    );
    class EngineChild;
    class Engine {
        board::Board &brd;
        types::PieceColour self;
        eval::Evaluator evaluator;
        double MAXTIME; // In total we want to spend 10 seconds per move 
        transpo::TranspositionTable trans;
        // Interrupt during each step of search? Time calculation could be expensive
        // Interrupt at top most layer? Could produce inaccurate results
        
        public:
            Engine(
                board::Board &b,
                types::PieceColour p = types::PieceColour::RED,
                double maxTime = 5.0) :
            brd(b),
            self(p),
            MAXTIME(maxTime)
            {}
            board::Board getBoard() const {
                return brd; 
            }
            constexpr types::PieceColour getColour() const {
                return self;
            }
            constexpr eval::Evaluator getEvaluator() const {
                return evaluator;
            }
            transpo::TranspositionTable &getTrans() {
                return trans;
            }


            types::Move chooseNextMove() {
                // MULTITHREADING STEPS:
                // MAIN THREAD:
                // GENERATE ALL LEGAL MOVES
                // FOR EACH LEGAL MOVE POP IT ONTO A WORK STACK
                // CREATE ARRAY WITH LENGTH = NUMBER OF MOVES
                // SPAWN AND START N THREADS
                // HANG WHILE TIME LEFT 
                // KILL THREADS WHEN TIME HAS RAN OUT

                // THREAD PROCESS:
                // POP OFF THE MOVE FROM THE WORKSTACK
                // PLAY THE MOVE AND PERFORM ITERATIVE DEEPENING
                // FOREACH ITERATION:
                // PERFORM THE SEARCH
                // STORE EVALUATION IN MASTER THREAD DS

                // PROBLEMS:
                // EACH THREAD REQUIRES ITS OWN BOARD INSTANCE
                // THREADS MAY SHARE TT -> CAUSES DATA CORRUPTION -> MUTEXES -> SLOW
                std::time_t start = std::time(nullptr);
                
                // generate legal moves
                if (brd.isPlayerCheckmate(self)) {
                    return types::Move();
                }
                std::vector<std::shared_ptr<types::Move>> moves;
                brd.generateLegalMoves(self, moves);

                if (moves.size() == 0) {
                    brd.setPlayerCheckmate(self);
                    return types::Move();
                }

                std::stack<unsigned int, std::list<unsigned int>> workStack {};
                unsigned int workCounter = 0;
                while (workCounter != moves.size()) {
                    workStack.push(workCounter++);
                }

                std::vector<std::int_fast16_t> evals {};
                evals.reserve(workStack.size());
                evals.insert(evals.end(), evals.capacity(), -9999999);
                // const unsigned short THREADSIZE = std::thread::hardware_concurrency();
                const unsigned short THREADSIZE = moves.size();
                std::vector<std::jthread> threadPool;
                threadPool.reserve(THREADSIZE);
                std::stop_source timeExceeded;
                // std::mutex workMutex;
                std::mutex initMutex;
                std::unique_lock initLock{initMutex};
                std::vector<std::reference_wrapper<EngineChild>> children;
                children.reserve(THREADSIZE);

                auto threadFunction = std::bind(startChild, this, std::ref(moves), std::ref(evals), std::ref(workStack), std::ref(initMutex), std::ref(timeExceeded));
                for (unsigned int i = 0; i < THREADSIZE; ++i) {
                    threadPool.emplace_back(std::jthread(threadFunction));
                }
                initLock.unlock();
                while (std::difftime(std::time(nullptr), start) < MAXTIME) {}
                timeExceeded.request_stop();
                
                return *moves[std::distance(evals.begin(), std::max_element(evals.begin(), evals.end()))];


            }
    };
    class EngineChild {
        board::Board board; // board for altering state
        types::PieceColour self; // colour for evaluation
        eval::Evaluator evaluator; // evaluator for evaluation
        std::vector<std::shared_ptr<types::Move>> &rootMoves; // for accessing the move to be played
        transpo::TranspositionTable &trans; // transposition table accessed by all threads
        std::vector<std::int_fast16_t> &out; // out vector to hold result of work
        std::stack<unsigned int, std::list<unsigned int>> &workStack; // work stack for holidng work to be done
        std::stop_source &source;
        std::stop_token timeExceeded; // stop when time exceeded

        transpo::Node existsTransTableCutoff(transpo::TableData data, unsigned int depth, std::int_fast16_t alpha, std::int_fast16_t beta) {
            if (data.occupied && data.depth >= depth) {
                switch(data.type) {
                    case transpo::Node::PV:
                        // std::cout << "transpo cutoff pv\n";
                        return transpo::Node::PV;
                    case transpo::Node::ALL:
                        if (data.eval < alpha) {
                            // std::cout << "transpo cutoff all\n";
                            return transpo::Node::ALL;
                        }
                        return transpo::Node::NONE;
                    case transpo::Node::CUT:
                        if (data.eval >= beta) {
                            // std::cout << "transpo cutoff cut\n";
                            return transpo::Node::CUT;
                        }
                        return transpo::Node::NONE;
                    default:
                        return transpo::Node::NONE;
                }
            }
            return transpo::Node::NONE;
        }   
        std::int_fast16_t alphaBetaMax(unsigned int depth, std::int_fast16_t alpha, std::int_fast16_t beta) {
            if (depth == 0) {
                std::array<std::int_fast16_t, 4UL> e = evaluator.getEvaluation(board, board.getPlayers());
                return 2 * e[helper::indexFromColour(self)] - std::accumulate(e.begin(), e.end(), 0);
            }
            
            std::int_fast16_t evaluation = 0;
            std::int_fast16_t bestEval = -99999999;
            types::Move bestMove;
            depth--;

            bool failLow = true;
            transpo::TableData data = trans.find(board.getPlayers());
            if (data.occupied) {
                const transpo::Node transCutoff = existsTransTableCutoff(data, depth, alpha, beta);
                if (transCutoff != transpo::Node::NONE) {
                    switch (transCutoff) {
                        case transpo::Node::PV:
                            return data.eval;
                        case transpo::Node::ALL:
                            if (data.eval < alpha) {
                                return data.eval;
                            }
                            break;
                        default:
                            if (data.eval >= beta) {
                                return data.eval;
                            }
                            break;
                    }
                }
                if (timeExceeded.stop_requested()) {
                    return bestEval;
                }
                board.playMove(data.bestMove);
                if (board.getCurrentTurn() == self) {
                    evaluation = alphaBetaMax(depth, alpha, beta);
                } else {
                    evaluation = alphaBetaMin(depth, alpha, beta);
                }
                board.unPlayMove();

                if (evaluation >= beta) {
                    // fail high
                    // node is CUT
                    trans.store(data.bestMove, evaluation, board.halfMoveClock, depth, transpo::Node::CUT, board.getPlayers());
                    return evaluation;
                }
                bestMove = data.bestMove;
                bestEval = evaluation;
                if (evaluation > alpha) {
                    failLow = false;
                    alpha = evaluation;
                }
            }
            
            // search rest of moves
            std::vector<std::shared_ptr<types::Move>> moves;
            board.generateLegalMoves(board.getCurrentTurn(), moves);
            for (unsigned int i = 0; i < moves.size(); ++i) {
                if (timeExceeded.stop_requested()) {
                    return bestEval;
                }
                if (data.bestMove.index != i) {
                    types::Move move = *moves[i];
                    board.playMove(move);
                    evaluation = alphaBetaMin(depth, alpha, beta);
                    board.unPlayMove();
                    
                    if (evaluation >= beta) {
                        // fail high
                        // node is CUT
                        trans.store(move, evaluation, board.halfMoveClock, depth, transpo::Node::CUT, board.getPlayers());
                        return evaluation;
                    }
                    if (evaluation > bestEval) {
                        bestMove = move;
                        bestEval = evaluation;
                        if (evaluation > alpha) {
                            failLow = false;
                            alpha = bestEval;
                        }
                    }
                }
            }

            if (failLow) {
                trans.store(bestMove, bestEval, board.halfMoveClock, depth, transpo::Node::ALL, board.getPlayers());
            } else {
                trans.store(bestMove, bestEval, board.halfMoveClock, depth, transpo::Node::PV, board.getPlayers());
            }
            return bestEval;
        }
        std::int_fast16_t alphaBetaMin(unsigned int depth, std::int_fast16_t alpha, std::int_fast16_t beta) {
            if (depth == 0) {
                std::array<std::int_fast16_t, 4UL> e = evaluator.getEvaluation(board, board.getPlayers());
                return 2 * e[helper::indexFromColour(self)] - std::accumulate(e.begin(), e.end(), 0);
            }

            bool failLow = true;
            transpo::TableData data = trans.find(board.getPlayers());
            if (data.occupied) {
                const transpo::Node transCutoff = existsTransTableCutoff(data, depth, alpha, beta);
                if (transCutoff != transpo::Node::NONE) {
                    switch (transCutoff) {
                        case transpo::Node::PV:
                            // no such cutoff ever 
                            // std::cout << "PV CUTOFF\n";
                            return data.eval;
                        case transpo::Node::ALL:
                            if (data.eval < alpha) {
                                // std::cout << "ALL CUTOFF\n";
                                return data.eval;
                            }
                            break;
                        default:
                            if (data.eval >= beta) {
                                // std::cout << "CUT CUTOFF\n";
                                return data.eval;
                            }
                            break;
                    }
                }
            }

            std::int_fast16_t evaluation;
            std::int_fast16_t bestEval = 99999999;
            types::Move bestMove;
            depth--;
            if (timeExceeded.stop_requested()) {
                return bestEval;
            }
            // there was data but not enough for a cutoff
            if (data.occupied) {
                board.playMove(data.bestMove);

                if (board.getCurrentTurn() == self) {
                    evaluation = alphaBetaMax(depth, alpha, beta);
                } else {
                    evaluation = alphaBetaMin(depth, alpha, beta);
                }
                board.unPlayMove();

                if (evaluation <= alpha) {
                    // fail high
                    // node is CUT
                    trans.store(data.bestMove, evaluation, board.halfMoveClock, depth, transpo::Node::CUT, board.getPlayers());
                    return evaluation;
                }
                bestMove = data.bestMove;
                bestEval = evaluation;
                if (evaluation < beta) {
                    failLow = false;
                    beta = evaluation;
                }
            }

            std::vector<std::shared_ptr<types::Move>> moves;
            board.generateLegalMoves(board.getCurrentTurn(), moves);

            for (unsigned int i = 0; i < moves.size(); i++) {
                if (timeExceeded.stop_requested()) {
                    return bestEval;
                }
                if (data.bestMove.index != i) {
                    types::Move move = *moves[i];
                    board.playMove(move);

                    if (board.getCurrentTurn() == self) {
                        evaluation = alphaBetaMax(depth, alpha, beta);
                    } else {
                        evaluation = alphaBetaMin(depth, alpha, beta);
                    }
                    board.unPlayMove();

                    if (evaluation <= alpha) {
                        // fail high
                        // node is CUT
                        trans.store(move, evaluation, board.halfMoveClock, depth, transpo::Node::CUT, board.getPlayers());
                        return evaluation;
                    }
                    if (evaluation < bestEval) {
                        bestEval = evaluation;
                        bestMove = move; // assignment not working257
                        if (evaluation < beta) {
                            failLow = false;
                            beta = evaluation;
                        }
                    }
                }
            }
            if (failLow) {
                trans.store(bestMove, bestEval, board.halfMoveClock, depth, transpo::Node::ALL, board.getPlayers());
            } else {
                trans.store(bestMove, bestEval, board.halfMoveClock, depth, transpo::Node::PV, board.getPlayers());
            }
            return bestEval;
        }

        public:
        EngineChild(
            engine::Engine *parent,
            std::vector<std::shared_ptr<types::Move>> &moves, 
            std::vector<std::int_fast16_t> &_out, 
            std::stack<unsigned int, std::list<unsigned int>> &workStack,
            // std::mutex &workMutex,
            std::stop_source &_timeExceeded
        ) : 
        board((*parent).getBoard()),
        self((*parent).getColour()),
        evaluator((*parent).getEvaluator()),
        rootMoves(moves),
        trans((*parent).getTrans()),
        out(_out),
        workStack(workStack),
        // workMutex(workMutex),
        source(_timeExceeded),
        timeExceeded(_timeExceeded.get_token())
        {}

        EngineChild &operator=(const EngineChild &other)
        {
            board = other.board;
            self = other.self;
            evaluator = other.evaluator;
            rootMoves = other.rootMoves;
            trans = other.trans;
            out = other.out;
            workStack = other.workStack;
            // workMutex = other.workMutex;
            source = other.source;
            timeExceeded = other.source.get_token();
            return *this;
        }
        // process for child threads 
        // child threads require atomic popping of shared workStack, a local board variable, shared access to evals  
        void evaluateBranch(std::mutex &initMutex) {
            // std::cout << "evaluating branch\n";
            // hangs while unique_lock is locked
            std::unique_lock tmpLock{initMutex};
            tmpLock.unlock();
            unsigned int moveIndex;
            std::unique_lock<std::mutex> workLock = std::unique_lock(engine::workMutex, std::defer_lock);
            while (!workStack.empty()){
                // blocking lock
                workLock.lock();
                moveIndex = workStack.top();
                workStack.pop();
                workLock.unlock();
                const types::Move move = *rootMoves[moveIndex];
                unsigned int depth = 0;
                // find current best in the evals
                while (!timeExceeded.stop_requested()) {
                    //iterative deepening
                    board.playMove(move);
                    out[moveIndex] = alphaBetaMin(depth, -99999999, 99999999);
                    board.unPlayMove();
                    depth++;
                }
            }
        }
    };

    void startChild(
        Engine *parent, 
        std::reference_wrapper<std::vector<std::shared_ptr<types::Move>>> moves, 
        std::reference_wrapper<std::vector<std::int_fast16_t>> evals, 
        std::reference_wrapper<std::stack<unsigned int, std::list<unsigned int>>> workStack,
        std::reference_wrapper<std::mutex> initMutex,
        std::reference_wrapper<std::stop_source> timeExceeded
    ) {
        thread_local EngineChild child = EngineChild(parent, moves, evals, workStack, timeExceeded);
        child.evaluateBranch(initMutex);
    }




};
#endif