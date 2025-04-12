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
#include<stop_token>
#include<mutex>

#include "bitboard.h"
#include"board.h"
#include"helper.h"
#include"evaluator.h"
#include "move.h"
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
        public:
            Engine(board::Board &board, types::PieceColour colour) :
                m_board(board),
                d_copy(board),
                m_selfColour(colour)
            {}

            move::Move chooseNextMove() {
                debug = 0;
                const std::vector<move::Move> playableMoves = m_board.generateLegalMoves(m_selfColour);

                assert(m_board == d_copy);

                unsigned int depth = 0;
                unsigned int bestIndex = 0;

                std::int_fast16_t beta = INT_FAST16_MAX;

                while (!timeUp()) {
                    std::int_fast16_t bestEval = evalBest(playableMoves[bestIndex], INT_FAST16_MIN, beta, depth, MIN);

                    for (int i = 0; i < playableMoves.size(); i++) {
                        std::cout << "evaluating move " << i << "\n";
                        move::Move currentMove = playableMoves[i];
                        m_board.playMove(currentMove);

                        const std::int_fast16_t currentEval = alphaBetaMin(bestEval, beta, depth);

                        if (currentEval > bestEval) {
                            bestEval = currentEval;
                            bestIndex = i;
                        }

                        m_board.unPlayMove();
                        assert(m_board == d_copy);
                    }
                    depth++;
                }

                assert(m_board == d_copy);
                return playableMoves[bestIndex];
            }

            // evaluates an individual move
            std::int_fast16_t evalMove(const move::Move p_move) {
                debug = 0;
                m_board.playMove(p_move);

                std::int_fast16_t bestEval = INT_FAST16_MIN;
                std::int_fast16_t beta = INT_FAST16_MAX;
                unsigned int bestIndex = 0;

                int depth = 0;

                const std::vector<move::Move> playableMoves = m_board.generateLegalMoves(m_board.getCurrentTurn());

                while (!timeUp()) {
                    bestEval = evalBest(playableMoves[bestIndex], INT_FAST16_MIN, beta, depth -1, MIN);
                    beta = INT_FAST16_MAX;

                    for (unsigned int i = 0; i < playableMoves.size(); i++) {
                        std::cout << "evaluating move " << i << "\n";
                        const move::Move currentMove = playableMoves[i];
                        m_board.playMove(currentMove);

                        const std::int_fast16_t currentEval = alphaBetaMin(bestEval, beta, depth - 1);

                        if (currentEval > bestEval) {
                            bestEval = currentEval;
                            bestIndex = i;
                        }

                        m_board.unPlayMove();
                    }
                    depth++;
                }

                m_board.unPlayMove();
                assert(m_board == d_copy);
                return bestEval;
            }

            board::Board getBoard() {
                return m_board;
            }
            types::PieceColour getColour() {
                return m_selfColour;
            }
        private:
            int debug = 0;
            board::Board d_copy;
            board::Board &m_board;
            eval::Evaluator m_evaluator;
            types::PieceColour m_selfColour;
            std::int_fast16_t evalBest(move::Move p_best, std::int_fast16_t p_alpha, std::int_fast16_t p_beta, unsigned int p_depth, int p_mode) {
                m_board.playMove(p_best);
                if (p_depth == 0) { 
                    return m_evaluator.evalParanoid(m_board, m_selfColour);
                }
                std::int_fast16_t eval = 0;
                switch (p_mode) {
                    case MIN:
                        eval = alphaBetaMin(p_alpha, p_beta, p_depth - 1);
                    case MAX:
                        eval = alphaBetaMax(p_alpha, p_beta, p_depth - 1);
                    default:
                        // how did we get here?
                        assert(false);
                }
                m_board.unPlayMove();
                return eval;
            }

            int timeUp() {
                std::cout << "incrementing clock " << debug << "\n";
                debug++;
                return debug == 2;
            }

            std::int_fast16_t alphaBetaMax(int p_alpha, int p_beta, int p_depth) {
                // std::cout << "maximising\n";
                if (p_depth == 0) {
                    return m_evaluator.evalParanoid(m_board, m_selfColour); 
                }
                std::int_fast16_t bestEval = INT_FAST16_MIN;
                const std::vector<move::Move> playableMoves = m_board.generateLegalMoves(m_board.getCurrentTurn());

                for (int i = 0; i < playableMoves.size(); i++) {
                    const move::Move currentMove = playableMoves[i];
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
                if (p_depth == 0) {
                    return m_evaluator.evalParanoid(m_board, m_selfColour);
                }
                std::int_fast16_t bestEval = INT_FAST16_MAX;
                const std::vector<move::Move> playableMoves = m_board.generateLegalMoves(m_board.getCurrentTurn());

                for (int i = 0; i < playableMoves.size(); i++) {
                    const move::Move currentMove = playableMoves[i];
                    m_board.playMove(currentMove);

                    const std::int_fast16_t currentEval = getNextMiniMax(p_alpha, p_beta, p_depth - 1);

                    if (currentEval < p_alpha) {
                        // fail low
                        std::cout << "fail low\n";
                        return currentEval;
                    }
                    if (currentEval > bestEval) {
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
