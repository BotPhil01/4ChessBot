#include<iostream>
#include<vector>
#include<algorithm>
#include<exception>
#include<numeric>
#include<cstdint>
#include<ctime>
#include<fstream>

#include"board.h"
#include"helper.h"
#include"evaluator.h"
#include"transpo.h"

#ifndef ENGINE_H
#define ENGINE_h

namespace engine {
    
    class Engine {
        board::Board &board;
        types::PieceColour self;
        eval::Evaluator evaluator;
        const double MAXTIME = 10.0; // In total we want to spend 10 seconds per move 
        transpo::TranspositionTable trans = transpo::TranspositionTable();
        unsigned int collisions = 0;

            // Interrupt during each step of search? Time calculation could be expensive
            // Interrupt at top most layer? Could produce inaccurate results


        public:
            Engine(
                board::Board &b,
                types::PieceColour p = types::PieceColour::RED, 
                bool finished = false) :
            board(b),
            self(p)
            {
            }
            
            types::PieceColour getColour() {
                return self;
            }

            constexpr transpo::Node existsTransTableCutoff(transpo::TableData data, unsigned int depth, std::int_fast16_t alpha, std::int_fast16_t beta) const {
                if (data.occupied && data.depth >= depth) {
                    switch(data.type) {
                        case transpo::Node::PV:
                            return transpo::Node::PV;
                        case transpo::Node::ALL:
                            return data.eval < alpha ? transpo::Node::ALL : transpo::Node::NONE;
                        case transpo::Node::CUT:
                            return data.eval >= beta ? transpo::Node::CUT : transpo::Node::NONE;
                        default:
                            return transpo::Node::NONE;
                    }
                }
                return transpo::Node::NONE;
            }



            // returns a next move 
            // implements iterative deepening
            types::Move chooseNextMove() {
                std::time_t start = std::time(nullptr);
                
                // generate legal moves
                if (board.isPlayerCheckmate(self)) {
                    return types::Move();
                }
                std::vector<std::unique_ptr<types::Move>> moves;
                board.generateLegalMoves(self, moves);

                if (moves.size() == 0) {
                    board.setPlayerCheckmate(self);
                    return types::Move();
                }

                //iterative deepening
                types::Move bestMove = *moves[0];
                unsigned int depth = 0;
                // TODO CHANGE  TO INCORPORATE SIGNALS INSTEAD OF CALCULATION
                // REQUIRES A TIMER THREAD
                while (std::difftime(std::time(nullptr), start) < MAXTIME) {

                    std::int_fast16_t bestEval = -99999999;
                    std::int_fast16_t beta = 99999999;
                    // search bestIndex
                    board.playMove(bestMove);
                    bestEval = alphaBetaMin(depth, bestEval, beta);
                    board.unPlayMove();
                    

                    // search rest
                    for (unsigned int i = 0; i < moves.size(); i++) {
                        if (std::difftime(std::time(nullptr), start) >= MAXTIME) {
                            break;
                        }
                        if (i != bestMove.index){
                            types::Move move = *moves[i];
                            board.playMove(move);
                            std::int_fast16_t evaluation = alphaBetaMin(depth, bestEval, beta);
                            
                            if (evaluation > bestEval) {
                                bestMove = move;
                                bestEval = evaluation;
                            }
        
                            board.unPlayMove();
                        }
                    }
                    trans.store(bestMove, bestEval, depth, transpo::Node::PV, board.getPlayers());
                    depth++;
                }


                {
                    std::ofstream ostrm("logs.txt", std::ios::app);
                    ostrm << "colour: " << helper::colourToChar(self) << " collisions: " << collisions << std::endl; 
                    ostrm.close();
                }
                return bestMove;


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
                    collisions++;
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
                        trans.store(data.bestMove, evaluation, depth, transpo::Node::CUT, board.getPlayers());
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
                std::vector<std::unique_ptr<types::Move>> moves;
                board.generateLegalMoves(board.getCurrentTurn(), moves);
                for (unsigned int i = 0; i < moves.size(); ++i) {
                    if (data.bestMove.index != i) {
                        types::Move move = *moves[i];
                        board.playMove(move);
                        evaluation = alphaBetaMin(depth, alpha, beta);
                        board.unPlayMove();
                        
                        if (evaluation >= beta) {
                            // fail high
                            // node is CUT
                            trans.store(move, evaluation, depth, transpo::Node::CUT, board.getPlayers());
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
                    trans.store(bestMove, bestEval, depth, transpo::Node::ALL, board.getPlayers());
                } else {
                    trans.store(bestMove, bestEval, depth, transpo::Node::PV, board.getPlayers());
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
                    collisions++;
                    const transpo::Node transCutoff = existsTransTableCutoff(data, depth, alpha, beta);
                    if (transCutoff != transpo::Node::NONE) {
                        switch (transCutoff) {
                            case transpo::Node::PV:
                                // no such cutoff ever 
                                std::cout << "PV CUTOFF\n";
                                return data.eval;
                            case transpo::Node::ALL:
                                if (data.eval < alpha) {
                                    std::cout << "ALL CUTOFF\n";
                                    return data.eval;
                                }
                                break;
                            case transpo::Node::CUT:
                                if (data.eval >= beta) {
                                    std::cout << "CUT CUTOFF\n";
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
                        trans.store(data.bestMove, evaluation, depth, transpo::Node::CUT, board.getPlayers());
                        return evaluation;
                    }
                    bestMove = data.bestMove;
                    bestEval = evaluation;
                    if (evaluation < beta) {
                        failLow = false;
                        beta = evaluation;
                    }
                }

                std::vector<std::unique_ptr<types::Move>> moves;
                board.generateLegalMoves(board.getCurrentTurn(), moves);

                for (unsigned int i = 0; i < moves.size(); i++) {
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
                            trans.store(move, evaluation, depth, transpo::Node::CUT, board.getPlayers());
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
                    trans.store(bestMove, bestEval, depth, transpo::Node::ALL, board.getPlayers());
                } else {
                    trans.store(bestMove, bestEval, depth, transpo::Node::PV, board.getPlayers());
                }
                return bestEval;
            }


            
    };
};
#endif