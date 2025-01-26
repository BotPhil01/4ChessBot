
#include<iostream>
#include<vector>
#include<algorithm>
#include<exception>
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
                int depth = 2, 
                bool finished = false) :
            board(b),
            self(p), 
            DEPTH(depth)
            {
                
            }
            types::PieceColour getColour() {
                return self;
            }

            // returns a next move 
            types::Move chooseNextMove() { 
                // std::cout << "choosing next move" << std::endl;
                // return the next move or an empty move from default move constructor if unable to find one
                // TODO ADAPT FOR MULTIPLE OPPONENT NATURE OF 4PCHESS -> want the algorithm to be able to check mid way through whether a new player becomes the strongest
                // main idea could be after an upper bound of advantage gained it reevaluates
                // alternatively give a lower depth -> makes sense as 4PChess has higher variance than regular games
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
                
                // store the generated move length 
                // play a move 
                float bestDiff = -99999999.0f;
                types::Move bestMove;
                for (std::unique_ptr<types::Move> &m : moves) {
                    board.playMove(*m);
                    std::array<float, 4UL> eval = search(DEPTH);
                    
                    float enemyBest = -9999999.0f;
                    for (unsigned int i = 0; i < eval.size(); ++i) {
                        if (i != helper::indexFromColour(self) && eval[i] > enemyBest) {
                            enemyBest = eval[i];
                        }
                    }

                    float diff = eval[helper::indexFromColour(self)] - enemyBest;
                    if (diff > bestDiff) {
                        bestDiff = diff;
                        bestMove = *m;
                    }
                    board.unPlayMove();
                }
                return bestMove;
            }

            std::array<float, 4UL> search(unsigned int depth) {
                if (depth == 0) {
                    // replace with quiesce later
                    return eval.getEvaluation(board, board.getPlayers());
                }

                types::PieceColour us = board.getCurrentTurn();
                std::vector<std::unique_ptr<types::Move>> moves;
                board.generateLegalMoves(us, moves);

                float bestDiff = -99999999.0f;
                std::array<float, 4UL> bestEval = {0, 0, 0, 0};
                types::Move bestMove; 
                for (unsigned int i = 0; i < moves.size(); ++i) {
                    types::Move m = *(moves[i]);
                    board.playMove(m);
                    std::array<float, 4UL> eval = search(depth-1);

                    float enemyBest = -99999999.0f;
                    for (unsigned int i = 0; i < eval.size(); ++i) {
                        if (i != helper::indexFromColour(us) && eval[i] > enemyBest) {
                            enemyBest = eval[i];
                        }
                    }
                    float diff = eval[helper::indexFromColour(us)] - enemyBest;
                    if (diff > bestDiff) {
                        bestDiff = diff;
                        bestMove = m;
                        bestEval = eval;
                    }
                    board.unPlayMove();
                }
                return bestEval;
            }

            // evaluates current position by maximising our position
            // currently quite passive
            float evaluateBoard(types::PieceColour col) {
                unsigned int i = helper::indexFromColour(col);
                return eval.getEvaluation(board, board.getPlayers())[i];
            }
            
            
    };
};
#endif