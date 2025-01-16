
#include<iostream>
#include<vector>
#include<algorithm>
#include<exception>
#include"board.h"
#include"helper.h"

using namespace board;
using namespace helper;
using namespace std;

#ifndef ENGINE_H
#define ENGINE_h

namespace engine {
    class Engine : public Board {
        private:
            PieceColour self;
            int DEPTH = 2;

            // returns material evaluation for all colours 
            // vector in form {r, b, y, g}
            const std::vector<float> evaluateMaterial() {
                std::vector<float> weightings {
                    getMaterial(PieceColour::RED), 
                    getMaterial(PieceColour::BLUE), 
                    getMaterial(PieceColour::YELLOW), 
                    getMaterial(PieceColour::GREEN)}; //r b y g
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
            const std::vector<float> evaluateMobility() {
                std::vector<float> out;
                for (PieceColour c : helper::playableColours) {
                    if (!movesPlayed()) { // failed to find move
                        out.emplace_back((float) generateLegalMoves(c).size());
                        continue;
                    }
                    out.emplace_back(getLastMove(c).totalMoves);
                }
                return out;
            }
            // search to a depth
            // alpha =  lower bound
            // beta = upper bound
            // search for better move
            // if better move is found then bring alpha up to better move's score
            // if the minbound exceeds maxbound it wont be considered
            float alphaBetaMax(float alpha, float beta, int depth, PieceColour commonEnemy) {
                // TODO ADAPT FOR MULTIPLE OPPONENT NATURE OF 4PCHESS
                if (depth == 0) {
                    // return quiesce
                    return evaluateBoard().second;
                }
                auto moves = generateLegalMoves(getCurrentTurn());
                for (auto move : moves) {
                    move.setTotal(moves.size());
                    playMove(move);
                    float res = alphaBetaMin(alpha, beta, depth-1, commonEnemy);
                    if (res > alpha && res < beta) { // found a better move
                        alpha = res; // new lower bound is our best move
                    } else if (res > alpha && res >= beta) { // our better move wont be considered by the opponent
                        unPlayMove();
                        return beta;
                    }
                    unPlayMove();
                }
                return alpha;
            }

            // alpha =  lower bound
            // beta = upper bound
            float alphaBetaMin(float alpha, float beta, int depth, PieceColour commonEnemy) { //tries to minimise the value
                // TODO ADAPT FOR MULTIPLE OPPONENT NATURE OF 4PCHESS
                if (depth == 0) {
                    // return quiesce
                    return evaluateBoard().second;
                } 
                auto moves = generateLegalMoves(getCurrentTurn());
                
                for (auto move : moves) {
                    move.setTotal(moves.size());
                    playMove(move);
                    float res = alphaBetaMax(alpha, beta, depth-1, commonEnemy);
                    if (res < beta) {
                        beta = res; // new min found
                        if (res < alpha) { // wont be considered
                            unPlayMove();
                            return alpha;
                        }
                    }
                    unPlayMove();
                }
                return beta;
            }


        public:
            bool hasFinished;
            Engine(PieceColour p = PieceColour::RED, int depth = 2, bool finished = false) :
            self(p), DEPTH(depth), hasFinished(finished), Board(){
                
            }
            PieceColour getColour() {
                return self;
            }

            // returns a next move 
            Move chooseNextMove() { 
                // return the next move or an empty move from default move constructor if unable to find one
                // TODO ADAPT FOR MULTIPLE OPPONENT NATURE OF 4PCHESS -> want the algorithm to be able to check mid way through whether a new player becomes the strongest
                // main idea could be after an upper bound of advantage gained it reevaluates
                // alternatively give a lower depth -> makes sense as 4PChess has higher variance than regular games
                // generate legal moves
                auto moves = generateLegalMoves(self);
                if (moves.size() == 0) {
                    hasFinished = true;
                    return Move();
                }
                PieceColour strongest = evaluateBoard().first;
                
                // store the generated move length 
                // play a move 
                float bestCutOff = 999;
                float bestEval = (float) -9999;
                Move bestMove = moves[0];
                for (Move m : moves) {
                    // m.totalMoves = movesLength;
                    playMove(m);
                    float eval = alphaBetaMax(-9999, 9999, DEPTH, strongest);
                    if (eval > bestEval) {
                        bestEval = eval;
                        bestMove = m;
                        if (eval > bestCutOff) {
                            unPlayMove();
                            break;
                        }
                    }
                    unPlayMove();
                }
                bestMove.totalMoves = moves.size();
                return bestMove;
            }

            // evaluates current board position
            // evaluation works by calculating the advantages based on material and position and choosing one player to focus
            // basically constantly tries to bring down the best player (other than itself) and assumes others will too!
            // returns the common enemy and the advantage difference
            std::pair<PieceColour, float> evaluateBoard() {
                auto material = evaluateMaterial();
                multiplyValues(&material, (float) 10); // weightings
                auto mobility = evaluateMobility();
                if (mobility[indexFromColour(self)] == 0) {
                    return pair(PieceColour::NONE, -999.0);
                }
                // auto position = evaluatePosition();
                auto advantages = layer(material, mobility); // combine into singular valus
                char selfIndex = getColourIndex(self);
                char maxAdvantageIndex = 0; // index of max advantage
                for (char i = 0; i < advantages.size(); i++) {
                    if (selfIndex != i && advantages[i] > advantages[maxAdvantageIndex]) {
                        maxAdvantageIndex = i;
                    }
                }
                return std::pair<PieceColour, float> (getColourFromIndex(maxAdvantageIndex), advantages[selfIndex] - advantages[maxAdvantageIndex]);
            }

            
    };
};
#endif