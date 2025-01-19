
#include<iostream>
#include<vector>
#include<algorithm>
#include<exception>
#include"board.h"
#include"helper.h"

#ifndef ENGINE_H
#define ENGINE_h

namespace engine {
    class Engine {
        private:
            board::Board &board;
            types::PieceColour self;
            int DEPTH;
            // returns material evaluation for all colours 
            // vector in form {r, b, y, g}
            const std::array<float, 4> evaluateMaterial() {
                std::array<float, 4> weightings {
                    board.getMaterial(types::PieceColour::RED), 
                    board.getMaterial(types::PieceColour::BLUE), 
                    board.getMaterial(types::PieceColour::YELLOW), 
                    board.getMaterial(types::PieceColour::GREEN)}; //r b y g
                return weightings;   
            }

            // const std::vector<float> evaluatePosition() {
            //     // fundamental ideas
            //     // pawns close to center = good
            //     // developed position = good
            //     // doubled, isolated and blocked pawns is bad
            //     // ideally the cost of pushing a pawn to the center but making it isolated should be greater than not pushing and reinforcing
            //     // TODO implement later as an extra
            // }

            // vector in form {r, b, y, g}
            const std::array<float, 4> evaluateMobility() {
                std::array<float, 4> out = {0, 0, 0, 0};
                std::vector<std::unique_ptr<types::Move>> tmp;
                for (unsigned char i = 0; i < helper::playableColours.size(); ++i) {
                    types::PieceColour c = helper::playableColours[i];
                    if (!board.movesPlayed()) { // failed to find move
                        board.generateLegalMoves(c, tmp);
                        out[i] = (float) tmp.size();
                        continue;
                    }
                    out[i] = (float) board.getLastMove(c).totalMoves;
                }
                return out;
            }

            // TODO this algorithm doesn't make sense
            // we need to maximise if the current enemy isnt us and minimise if the commonEnemy is us
            // search to a depth
            // alpha =  lower bound
            // beta = upper bound
            // search for better move
            // if better move is found then bring alpha up to better move's score
            // if the minbound exceeds maxbound it wont be considered
            float alphaBetaMax(float alpha, float beta, int depth, types::PieceColour commonEnemy) {
                // TODO ADAPT FOR MULTIPLE OPPONENT NATURE OF 4PCHESS
                if (depth == 0) {
                    // return quiesce
                    return evaluateBoard().second;
                }
                std::vector<std::unique_ptr<types::Move>> moves;
                board.generateLegalMoves(board.getCurrentTurn(), moves);
                if (moves.size() == 0) {
                    // no moves able to be generated
                    return evaluateBoard().second;
                }
                for (std::unique_ptr<types::Move> &move : moves) {
                    move->totalMoves = moves.size();
                    board.playMove(*move);
                    float res = alphaBetaMin(alpha, beta, depth-1, commonEnemy);
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
            float alphaBetaMin(float alpha, float beta, int depth, types::PieceColour commonEnemy) { //tries to minimise the value
                // TODO ADAPT FOR MULTIPLE OPPONENT NATURE OF 4PCHESS
                if (depth == 0) {
                    // return quiesce
                    return evaluateBoard().second;
                } 
                std::vector<std::unique_ptr<types::Move>> moves;
                board.generateLegalMoves(board.getCurrentTurn(), moves); 
                if (moves.size() == 0) {
                    // no moves able to be generated
                    return evaluateBoard().second;
                }
                
                for (std::unique_ptr<types::Move> &move : moves) {
                    move->totalMoves = moves.size();
                    board.playMove(*move);
                    float res = alphaBetaMax(alpha, beta, depth-1, commonEnemy);
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
                types::PieceColour strongest = evaluateBoard().first;
                
                // store the generated move length 
                // play a move 
                float bestCutOff = 999;
                float bestEval = (float) -9999;
                types::Move bestMove = *(moves[0]);
                for (std::unique_ptr<types::Move> &m : moves) {
                    // m.totalMoves = movesLength;
                    board.playMove(*m);
                    float eval = alphaBetaMax(-9999, 9999, DEPTH, strongest);
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

            // evaluates current board position
            // evaluation works by calculating the advantages based on material and position and choosing one player to focus
            // basically constantly tries to bring down the best player (other than itself) and assumes others will too!
            // returns the common enemy and the advantage difference
            std::pair<types::PieceColour, float> evaluateBoard() {
                auto material = evaluateMaterial();
                helper::multiplyValues(material, (float) 10); // weightings
                auto mobility = evaluateMobility();
                if (mobility[helper::indexFromColour(self)] == 0) {
                    return std::pair(types::PieceColour::NONE, -999.0);
                }
                // auto position = evaluatePosition();
                auto advantages = helper::layer(material, mobility); // combine into singular valus
                unsigned int selfIndex = helper::getColourIndex(self);
                unsigned int maxAdvantageIndex = 0; // index of max advantage
                for (unsigned int i = 0; i < advantages.size(); i++) {
                    if (selfIndex != i && advantages[i] > advantages[maxAdvantageIndex]) {
                        maxAdvantageIndex = i;
                    }
                }
                return std::pair<types::PieceColour, float> (helper::getColourFromIndex(maxAdvantageIndex), advantages[selfIndex] - advantages[maxAdvantageIndex]);
            }

            
    };
};
#endif