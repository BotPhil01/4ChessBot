#include <functional>
#include<iostream>
#include<limits.h>
#include<vector>
#include<algorithm>
#include<exception>
#include<numeric>
#include<cstdint>
#include<ctime>
#include<fstream>
#include<thread>
#include<mutex>
#include<stop_token>

#include "bitboard.h"
#include"board.h"
#include"helper.h"
#include"evaluator.h"
#include "move.h"
#include "timer.h"
// #include"transpo.h"

#ifndef ENGINE_H
#define ENGINE_h

namespace engine {
    // want an engine 
    // want a master process
    // need to use alpha beta
    // paranoid evaluation is already implemented
    namespace {
        const int MIN = 0;
        const int MAX = 1;
    }
    class Engine {
        int debug = 0;
        board::Board m_board;
        eval::Evaluator m_evaluator;
        types::PieceColour m_selfColour;
        std::reference_wrapper<bool> m_finishedFlag;
        public:
            Engine(board::Board p_board, types::PieceColour p_colour, std::reference_wrapper<bool> p_finishedFlag) :
                m_board(p_board),
                m_selfColour(p_colour),
                m_finishedFlag(p_finishedFlag)
            {}

            move::Move chooseNextMove() {
                debug = 0;
                std::vector<move::Move> playableMoves = m_board.generateLegalMoves(m_selfColour);


                unsigned int depth = 0;
                unsigned int bestIndex = 0;

                std::int_fast16_t beta = INT_FAST16_MAX;

                while (!timeUp()) {
                    std::int_fast16_t bestEval = evalBest(playableMoves[bestIndex], INT_FAST16_MIN, beta, depth, MIN);

                    for (int i = 0; i < playableMoves.size(); i++) {
                        move::Move &currentMove = playableMoves[i];
                        m_board.playMove(currentMove);

                        const std::int_fast16_t currentEval = alphaBetaMin(bestEval, beta, depth);

                        if (currentEval > bestEval) {
                            bestEval = currentEval;
                            bestIndex = i;
                        }

                        m_board.unPlayMove();
                    }
                    depth++;
                }

                return playableMoves[bestIndex];
            }

            void print() {
                m_board.print();
            }

            std::int_fast16_t evalIndex(const unsigned int p_moveIndex) {
                const std::vector<move::Move> playableMoves = m_board.generateLegalMoves(m_board.getCurrentTurn());
                const move::Move move = playableMoves[p_moveIndex];
                return evalMove(move);
            }

            // evaluates an individual move
            std::int_fast16_t evalMove(move::Move p_move) {
                debug = 0;
                m_board.playMove(p_move); // corrupts the board?

                std::int_fast16_t bestEval = INT_FAST16_MIN;
                std::int_fast16_t beta = INT_FAST16_MAX;
                unsigned int bestIndex = 0;

                int depth = 0;

                const std::vector<move::Move> playableMoves = m_board.generateLegalMoves(m_board.getCurrentTurn());

                while (!timeUp()) {
                    move::Move bestMove = playableMoves[bestIndex];
                    bestEval = evalBest(bestMove, INT_FAST16_MIN, beta, depth, MIN);
                    beta = INT_FAST16_MAX;

                    for (unsigned int i = 0; i < playableMoves.size(); i++) {
                        move::Move currentMove = playableMoves[i];
                        m_board.playMove(currentMove);

                        const std::int_fast16_t currentEval = alphaBetaMin(bestEval, beta, depth);

                        if (currentEval > bestEval) {
                            bestEval = currentEval;
                            bestIndex = i;
                        }

                        m_board.unPlayMove();
                    }
                    depth++;
                }

                m_board.unPlayMove();
                return bestEval;
            }

            board::Board getBoard() {
                return m_board;
            }
            types::PieceColour getColour() {
                return m_selfColour;
            }
        private:
            std::int_fast16_t evalBest(move::Move &p_best, std::int_fast16_t p_alpha, std::int_fast16_t p_beta, unsigned int p_depth, int p_mode) {
                m_board.playMove(p_best);
                if (p_depth == 0) { 
                    return m_evaluator.evalParanoid(m_board, m_selfColour);
                }
                std::int_fast16_t eval = 0;
                switch (p_mode) {
                    case MIN:
                        eval = alphaBetaMin(p_alpha, p_beta, p_depth - 1);
                        break;
                    case MAX:
                        eval = alphaBetaMax(p_alpha, p_beta, p_depth - 1);
                        break;
                    default:
                        // how did we get here?
                        assert(false);
                }
                m_board.unPlayMove();
                return eval;
            }

            bool timeUp() const {
                // check if the its time up
                return m_finishedFlag.get();
            }

            std::int_fast16_t alphaBetaMax(int p_alpha, int p_beta, int p_depth) {
                if (p_depth == 0 || timeUp()) {
                    return m_evaluator.evalParanoid(m_board, m_selfColour); 
                }
                std::int_fast16_t bestEval = INT_FAST16_MIN;
                const std::vector<move::Move> playableMoves = m_board.generateLegalMoves(m_board.getCurrentTurn());

                for (int i = 0; i < playableMoves.size(); i++) {
                    move::Move currentMove = playableMoves[i];
                    m_board.playMove(currentMove);

                    const std::int_fast16_t currentEval = alphaBetaMin(p_alpha, p_beta, p_depth - 1);

                    if (currentEval > p_beta) {
                        std::cout << "fail high\n";
                        // failhigh 
                        return currentEval;
                    }
                    if (currentEval > bestEval) {
                        bestEval = currentEval;
                        if (currentEval > p_alpha) {
                            p_alpha = currentEval;
                        }
                    }

                    m_board.unPlayMove();
                }
                return bestEval;
            }

            std::int_fast16_t alphaBetaMin(int p_alpha, int p_beta, int p_depth) {
                // std::cout << "minimising\n";
                if (p_depth == 0 || timeUp()) {
                    return m_evaluator.evalParanoid(m_board, m_selfColour);
                }
                std::int_fast16_t bestEval = INT_FAST16_MAX;
                const std::vector<move::Move> playableMoves = m_board.generateLegalMoves(m_board.getCurrentTurn());

                for (int i = 0; i < playableMoves.size(); i++) {
                    move::Move currentMove = playableMoves[i];
                    m_board.playMove(currentMove);

                    const std::int_fast16_t currentEval = getNextMiniMax(p_alpha, p_beta, p_depth - 1);

                    if (currentEval < p_alpha) {
                        // fail low
                        // std::cout << "fail low\n";
                        return currentEval;
                    }
                    if (currentEval < bestEval) {
                        bestEval = currentEval;
                        if (currentEval < p_beta) {
                            p_beta = currentEval;
                        }
                    }

                    m_board.unPlayMove();
                }
                return bestEval;
            }

            std::int_fast16_t getNextMiniMax (int p_alpha, int p_beta, int p_depth) {
                if (m_board.getCurrentTurn() == m_selfColour) {
                    return alphaBetaMax(p_alpha, p_beta, p_depth);
                }
                return alphaBetaMin(p_alpha, p_beta, p_depth);
            }
    };
};
#endif
