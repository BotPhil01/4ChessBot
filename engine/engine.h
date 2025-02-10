
#include<iostream>
#include<vector>
#include<algorithm>
#include<exception>
#include<numeric>
#include<cstdint>
#include<ctime>

#include"board.h"
#include"helper.h"
#include"evaluator.h"

#ifndef ENGINE_H
#define ENGINE_h

namespace engine {
    
    class Engine {
        private:
            board::Board &board;
            types::PieceColour self;
            eval::Evaluator evaluator;
            const double MAXTIME = 10.0; // In total we want to spend 10 seconds per move 
            int DEPTH;
            
            // Interrupt during each step of search? Time calculation could be expensive
            // Interrupt at top most layer? Could produce inaccurate results


        public:
            Engine(
                board::Board &b,
                types::PieceColour p = types::PieceColour::RED, 
                int d = 4, 
                bool finished = false) :
            board(b),
            self(p), 
            DEPTH(d)
            {
                
            }
            types::PieceColour getColour() {
                return self;
            }

            // returns a next move 
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
                types::Move bestMove;
                std::int_fast16_t bestEval = -99999999;
                std::int_fast16_t beta = 99999999;

                for (unsigned int i = 0; i < moves.size(); i++) {
                    std::time_t check = std::time(nullptr);
                    double difference = std::difftime(check, start);
                    if (difference >= MAXTIME) {
                        break;
                    }
                    types::Move m = *moves[i];
                    (*moves[i]).totalMoves = moves.size();
                    board.playMove(m);
                    std::int_fast16_t evaluation = alphaBetaMin(DEPTH, bestEval, beta);
                    
                    if (evaluation > bestEval) {
                        bestMove = m;
                        bestEval = evaluation;
                    }

                    board.unPlayMove();
                }
                return bestMove;


            }


            std::int_fast16_t alphaBetaMax(unsigned int depth, std::int_fast16_t alpha, std::int_fast16_t beta) {
                if (depth == 0) {
                    std::array<std::int_fast16_t, 4UL> e = evaluator.getEvaluation(board, board.getPlayers());
                    return 2 * e[helper::indexFromColour(self)] - std::accumulate(e.begin(), e.end(), 0);
                }

                std::vector<std::unique_ptr<types::Move>> moves;
                board.generateLegalMoves(board.getCurrentTurn(), moves);
                depth--;
                std::int_fast16_t bestEval = -99999999;

                for (unsigned int i = 0; i < moves.size(); i++) {
                    types::Move m = *moves[i];
                    (*moves[i]).totalMoves = moves.size();
                    board.playMove(m);
                    std::int_fast16_t evaluation = alphaBetaMin(depth, alpha, beta);
                    
                    if (evaluation >= beta) {
                        board.unPlayMove();
                        return evaluation;
                    }
                    if (evaluation > bestEval) {
                        bestEval = evaluation;
                        if (evaluation > alpha) {
                            alpha = bestEval;
                        }
                    }
                    board.unPlayMove();
                }
                return bestEval;
            }

            std::int_fast16_t alphaBetaMin(unsigned int depth, std::int_fast16_t alpha, std::int_fast16_t beta) {
                if (depth == 0) {
                    std::array<std::int_fast16_t, 4UL> e = evaluator.getEvaluation(board, board.getPlayers());
                    return 2 * e[helper::indexFromColour(self)] - std::accumulate(e.begin(), e.end(), 0);
                }


                std::vector<std::unique_ptr<types::Move>> moves;
                board.generateLegalMoves(board.getCurrentTurn(), moves);
                depth--;
                std::int_fast16_t bestEval = 99999999;
                std::int_fast16_t evaluation;

                for (unsigned int i = 0; i < moves.size(); i++) {
                    types::Move m = *moves[i];
                    (*moves[i]).totalMoves = moves.size();
                    board.playMove(m);

                    if (board.getCurrentTurn() == self) {
                        evaluation = alphaBetaMax(depth, alpha, beta);
                    } else {
                        evaluation = alphaBetaMin(depth, alpha, beta);
                    }

                    if (evaluation <= alpha) {
                        board.unPlayMove();
                        return evaluation;
                    }
                    if (evaluation < bestEval) {
                        bestEval = evaluation;
                        if (evaluation < beta) {
                            beta = evaluation;
                        }
                    }
                    board.unPlayMove();
                }
                return bestEval;
            }


            
    };
};
#endif