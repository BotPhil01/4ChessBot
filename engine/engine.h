
#include<iostream>
#include<vector>
#include<algorithm>
#include<exception>
#include"board.h"
#include"helper.h"
#include"evaluator.h"

#ifndef ENGINES_H
#define ENGINES_h

namespace engines {
    
    unsigned int DIFFENGINE = 0;
    unsigned int PARANOIDENGINE = 0;

    // engine interface
    class Engine {

        public:
            board::Board &BOARD;
            types::PieceColour SELF;
            unsigned int DEPTH;
            Engine(board::Board &board, types::PieceColour col, unsigned int depth) :
            BOARD(board),
            DEPTH(depth),
            SELF(col)
            {};
            virtual ~Engine(){}
            virtual types::Move chooseNextMove() = 0;
    };
    
     

    
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
    
    class DiffEngine : public Engine {
        private:
            eval::Evaluator eval;

            

        public:
            DiffEngine(
                board::Board &b,
                types::PieceColour p = types::PieceColour::RED, 
                int d = 2, 
                bool finished = false) :
            Engine(b, p, d)
            {
                
            }
            types::PieceColour getColour() {
                return SELF;
            }

            // returns a next move 
            types::Move chooseNextMove() { 
                // std::cout << "choosing next move" << std::endl;
                // return the next move or an empty move from default move constructor if unable to find one
                // TODO ADAPT FOR MULTIPLE OPPONENT NATURE OF 4PCHESS -> want the algorithm to be able to check mid way through whether a new player becomes the strongest
                // main idea could be after an upper bound of advantage gained it reevaluates
                // alternatively give a lower depth -> makes sense as 4PChess has higher variance than regular games
                // generate legal moves
                if (BOARD.isPlayerCheckmate(SELF)) {
                    return types::Move();
                }
                std::vector<std::unique_ptr<types::Move>> moves;
                BOARD.generateLegalMoves(SELF, moves);
                if (moves.size() == 0) {
                    BOARD.setPlayerCheckmate(SELF);
                    return types::Move();
                }
                
                // store the generated move length 
                // play a move 
                // float bestDiff = -99999999.0f;

                types::Move bestMove;
                unsigned int usIndex = helper::indexFromColour(SELF);
                SearchData data = SearchData(usIndex);
                float bestEval = -9999999999.0f;

                for (std::unique_ptr<types::Move> &m : moves) {
                    BOARD.playMove(*m);
                    std::array<float, 4UL> eval = search(DEPTH, data);
                    
                    // perform one search then check what thte state of the BOARD is 
                    // maximise us?
                    // assume we arent leader or second
                    // if we become leader then change data to be as such

                    if (eval[usIndex] > bestEval) {
                        bestMove = *m;
                        bestEval = eval[usIndex];
                    }

                    BOARD.unPlayMove();

                    // float enemyBest = -9999999.0f;
                    // for (unsigned int i = 0; i < eval.size(); ++i) {
                    //     if (i != helper::indexFromColour(SELF) && eval[i] > enemyBest) {
                    //         enemyBest = eval[i];
                    //     }
                    // }

                    // float diff = eval[helper::indexFromColour(SELF)] - enemyBest;
                    // if (diff > bestDiff) {
                    //     bestDiff = diff;
                    //     bestMove = *m;
                    // }
                    // BOARD.unPlayMove();
                }
                return bestMove;


            }

            // assumes data has been set in other words leader index and secondindex is known
            std::array<float, 4UL> search(unsigned int _depth, SearchData data) {
                if (_depth == 0) {
                    // replace with quiesce later
                    return eval.getDiffEvaluation(BOARD, BOARD.getPlayers());
                }

                const types::PieceColour us = BOARD.getCurrentTurn();
                const unsigned int usIndex = helper::indexFromColour(us);
                std::vector<std::unique_ptr<types::Move>> moves;
                BOARD.generateLegalMoves(us, moves);

                float bestDiff = -99999999.0f;
                std::array<float, 4UL> bestEval = {0, 0, 0, 0};
                bool haveAlternative = false;
                types::Move bestMove; 

                --_depth;
                for (unsigned int i = 0; i < moves.size(); ++i) {
                    types::Move m = *(moves[i]);
                    BOARD.playMove(m);
                    data.playedArray[usIndex] = true;

                    if (data.leaderIndex > 3 || data.secondIndex > 3) {
                        // leader and second haven't been set
                        std::array<float, 4UL> eval = search(_depth, data);
                        
                        unsigned char leaderIndex = 0;
                        for (unsigned int j = 0; j < eval.size(); ++j) {
                            if (eval[j] > eval[leaderIndex]) {
                                leaderIndex = j;
                            };
                        }

                        unsigned char secondIndex = 0;
                        for (unsigned int j = 0; j < eval.size(); ++j) {
                            if (j != leaderIndex && eval[j] > eval[secondIndex]) {
                                secondIndex = j;
                            }
                        }

                        data.leaderIndex = leaderIndex;
                        data.secondIndex = secondIndex;

                        if (eval[usIndex] > bestDiff) {
                            bestDiff = eval[usIndex];
                            bestMove = m;
                            bestEval = eval;
                            haveAlternative = true;
                        }

                        
                    } else if (data.leaderIndex != usIndex && data.secondIndex != usIndex) {
                        // we are not involved
                        std::array<float, 4UL> eval = search(_depth, data);

                        // alpha cutoff
                        if ((data.leaderPlayed() && eval[usIndex] > data.leaderArray[data.secondIndex]) 
                        ||  (data.secondPlayed() && eval[usIndex] > data.secondArray[data.secondIndex])) {
                            BOARD.unPlayMove();
                            return eval;
                        }

                        // maximise as normal
                        if (eval[usIndex] > bestDiff) {
                            bestDiff = eval[usIndex];
                            bestMove = m;
                            bestEval = eval;
                            haveAlternative = true;
                        }

                        
                        
                    } else if (data.leaderIndex == usIndex) {
                        // we are leader
                        if (haveAlternative) {
                            data.leaderArray = bestEval;
                        } else {
                            data.leaderArray = {99999.0f, 99999.0f, 99999.0f, 99999.0f};
                        }

                        std::array<float, 4UL> eval = search(_depth, data);

                        // alpha check
                        if (data.secondPlayed()) {
                            if (eval[data.leaderIndex] > data.secondArray[data.leaderIndex]) {
                                BOARD.unPlayMove();
                                return eval;
                            }
                        }

                        if (eval[usIndex] > bestDiff) {
                            bestDiff = eval[usIndex];
                            bestMove = m;
                            bestEval = eval;
                            haveAlternative = true;
                        }
                        
                        

                    } else if (data.secondIndex == usIndex) {
                        // we are second
                        if (haveAlternative) {
                            data.secondArray = bestEval;
                        } else {
                            data.secondArray = {99999.0f, 99999.0f, 99999.0f, 99999.0f};
                        }

                        std::array<float, 4UL> eval = search(_depth, data);

                        // alpha check
                        if (data.leaderPlayed()) {
                            if (eval[data.secondIndex] > data.leaderArray[data.secondIndex]) {
                                BOARD.unPlayMove();
                                return eval;
                            }
                        }

                        if (eval[usIndex] > bestDiff) {
                            bestDiff = eval[usIndex];
                            bestMove = m;
                            bestEval = eval;
                            haveAlternative = true;
                        }
                    }
                    BOARD.unPlayMove();
                }
                return bestEval;
            }
            
            
    };

    
};
#endif