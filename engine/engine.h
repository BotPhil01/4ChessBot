
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
            

            // TODO FIX THIS ALGORITHM
            // CURRENTLY THE CUTOFF IS WAY TOO SMALL AND BASICALLY JUST PICKS THE FIRST MOVE
            // we need to maximise if the current enemy isnt us and minimise if the commonEnemy is us
            // search to a depth
            // alpha =  lower bound
            // beta = upper bound
            // search for better move
            // if better move is found then bring alpha up to better move's score
            // if the minbound exceeds maxbound it wont be considered
            float alphaBetaMax(float alpha, float beta, int depth) {
                // TODO ADAPT FOR MULTIPLE OPPONENT NATURE OF 4PCHESS
                if (depth == 0) {
                    // return quiesce
                    return evaluateBoard(board.getCurrentTurn());
                }
                std::vector<std::unique_ptr<types::Move>> moves;
                board.generateLegalMoves(board.getCurrentTurn(), moves);
                if (moves.size() == 0) {
                    // no moves able to be generated
                    return evaluateBoard(board.getCurrentTurn());
                }
                for (std::unique_ptr<types::Move> &move : moves) {
                    move->totalMoves = moves.size();
                    board.playMove(*move);
                    float res = alphaBetaMin(alpha, beta, depth-1);
                    if (res > alpha && res < beta) { // found a better move
                        alpha = res; // new lower bound is our best move
                    } else if (res > alpha && res >= beta) { // our better move wont be considered by the opponent
                        board.unPlayMove();
                        return beta;
                    }
                    board.unPlayMove();
                }
                return alpha;
            }

            // alpha =  lower bound
            // beta = upper bound
            float alphaBetaMin(float alpha, float beta, int depth) { //tries to minimise the value
                // TODO ADAPT FOR MULTIPLE OPPONENT NATURE OF 4PCHESS
                if (depth == 0) {
                    // return quiesce
                    return evaluateBoard(board.getCurrentTurn());
                } 
                std::vector<std::unique_ptr<types::Move>> moves;
                board.generateLegalMoves(board.getCurrentTurn(), moves); 
                if (moves.size() == 0) {
                    // no moves able to be generated
                    return evaluateBoard(board.getCurrentTurn());
                }
                
                for (std::unique_ptr<types::Move> &move : moves) {
                    move->totalMoves = moves.size();
                    board.playMove(*move);
                    float res = alphaBetaMax(alpha, beta, depth-1);
                    if (res < beta) {
                        beta = res; // new min found
                        if (res < alpha) { // wont be considered
                            board.unPlayMove();
                            return alpha;
                        }
                    }
                    board.unPlayMove();
                }
                return beta;
            }


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
                float bestCutOff = 99999999.0f;
                float bestEval = -99999999.0f;
                types::Move bestMove = *(moves[0]);
                for (std::unique_ptr<types::Move> &m : moves) {
                    // m.totalMoves = movesLength;
                    board.playMove(*m);
                    float eval = alphaBetaMax(-9999, 9999, DEPTH);
                    if (eval > bestEval) {
                        bestEval = eval;
                        bestMove = *m;
                        if (eval > bestCutOff) {
                            board.unPlayMove();
                            break;
                        }
                    }
                    board.unPlayMove();
                }
                bestMove.totalMoves = moves.size();
                return bestMove;
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