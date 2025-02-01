
#include<iostream>
#include<vector>
#include<algorithm>
#include<exception>
#include<numeric>
#include<cstdint>

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
            eval::Evaluator eval;

            int DEPTH;
            

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
                std::int_fast16_t bestEval = -9999999999;
                std::int_fast16_t beta = 9999999;

                for (unsigned int i = 0; i < moves.size(); i++) {
                    types::Move m = *moves[i];
                    (*moves[i]).totalMoves = moves.size();
                    board.playMove(m);
                    std::int_fast16_t e = -alphaBetaMin(DEPTH, bestEval, beta);
                    
                    if (e > bestEval) {
                        bestMove = m;
                        bestEval = e;
                    }

                    board.unPlayMove();
                }
                return bestMove;


            }

            std::int_fast16_t alphaBetaMax(unsigned int depth, std::int_fast16_t alpha, std::int_fast16_t beta) {
                if (depth == 0) {
                    std::array<std::int_fast16_t, 4UL> e = eval.getEvaluation(board, board.getPlayers());
                    return 2 * e[helper::indexFromColour(self)] - std::accumulate(e.begin(), e.end(), 0);
                }

                std::vector<std::unique_ptr<types::Move>> moves;
                board.generateLegalMoves(board.getCurrentTurn(), moves);
                depth--;
                std::int_fast16_t bestEval = -99999;

                for (unsigned int i = 0; i < moves.size(); i++) {
                    types::Move m = *moves[i];
                    (*moves[i]).totalMoves = moves.size();
                    board.playMove(m);
                    std::int_fast16_t eval = -alphaBetaMin(depth, alpha, beta);
                    
                    if (eval >= beta) {
                        board.unPlayMove();
                        return eval;
                    }
                    if (eval > bestEval) {
                        bestEval = eval;
                        if (eval > alpha) {
                            alpha = bestEval;
                        }
                    }
                    board.unPlayMove();
                }
                return bestEval;
            }

            std::int_fast16_t alphaBetaMin(unsigned int depth, std::int_fast16_t alpha, std::int_fast16_t beta) {
                if (depth == 0) {
                    std::array<std::int_fast16_t, 4UL> e = eval.getEvaluation(board, board.getPlayers());
                    return std::accumulate(e.begin(), e.end(), 0) - 2 * e[helper::indexFromColour(self)];
                }


                std::vector<std::unique_ptr<types::Move>> moves;
                board.generateLegalMoves(board.getCurrentTurn(), moves);
                depth--;
                std::int_fast16_t bestEval = -99999;
                std::int_fast16_t eval;

                for (unsigned int i = 0; i < moves.size(); i++) {
                    types::Move m = *moves[i];
                    (*moves[i]).totalMoves = moves.size();
                    board.playMove(m);

                    if (board.getCurrentTurn() == self) {
                        eval = -alphaBetaMax(depth, alpha, beta);
                    } else {
                        eval = alphaBetaMin(depth, alpha, beta);
                    }

                    if (eval <= alpha) {
                        board.unPlayMove();
                        return eval;
                    }
                    if (eval < bestEval) {
                        bestEval = eval;
                        if (eval < beta) {
                            beta = eval;
                        }
                    }
                    board.unPlayMove();
                }
                return bestEval;
            }


            
    };
};
#endif