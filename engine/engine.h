
#include<iostream>
#include<vector>
#include<algorithm>
#include<exception>
#include<numeric>

#include"board.h"
#include"helper.h"
#include"evaluator.h"

#ifndef ENGINE_H
#define ENGINE_h

namespace engine {
    class SearchData {
        public:
            std::array<bool, 4UL> playedArray = {false, false, false, false};
            unsigned char leaderIndex = 4;
            unsigned char secondIndex = 4;
            std::array<float, 4UL> leaderArray = {0, 0, 0, 0};
            std::array<float, 4UL> secondArray = {0, 0, 0, 0};
            
            constexpr bool leaderPlayed() const {
                return leaderIndex != 4 && playedArray[leaderIndex];
            }
            constexpr bool secondPlayed() const {
                return secondIndex != 4 && playedArray[secondIndex];
            }
            SearchData(unsigned char cur) {
                playedArray[cur] = true;
            }
            SearchData() = default;


    };
    
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
                int d = 2, 
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
                float bestEval = -9999999999.0f;
                float beta = 9999999.0f;

                for (unsigned int i = 0; i < moves.size(); i++) {
                    types::Move m = *moves[i];
                    (*moves[i]).totalMoves = moves.size();
                    board.playMove(m);
                    float e = -alphaBetaMin(DEPTH, bestEval, beta);
                    
                    if (e > bestEval) {
                        bestMove = m;
                        bestEval = e;
                    }

                    board.unPlayMove();
                }
                return bestMove;


            }

            float alphaBetaMax(unsigned int depth, float alpha, float beta) {
                if (depth == 0) {
                    std::array<float, 4UL> e = eval.getEvaluation(board, board.getPlayers());
                    return 2 * e[helper::indexFromColour(self)] - std::accumulate(e.begin(), e.end(), 0);
                }

                std::vector<std::unique_ptr<types::Move>> moves;
                board.generateLegalMoves(board.getCurrentTurn(), moves);
                depth--;
                float bestEval = -99999.0f;

                for (unsigned int i = 0; i < moves.size(); i++) {
                    types::Move m = *moves[i];
                    (*moves[i]).totalMoves = moves.size();
                    board.playMove(m);
                    float eval = -alphaBetaMin(depth, alpha, beta);
                    
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

            float alphaBetaMin(unsigned int depth, float alpha, float beta) {
                if (depth == 0) {
                    std::array<float, 4UL> e = eval.getEvaluation(board, board.getPlayers());
                    return std::accumulate(e.begin(), e.end(), 0) - 2 * e[helper::indexFromColour(self)];
                }


                std::vector<std::unique_ptr<types::Move>> moves;
                board.generateLegalMoves(board.getCurrentTurn(), moves);
                depth--;
                float bestEval = -99999.0f;
                float eval;

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