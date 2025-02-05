#include"types.h"
#include"helper.h"
#include"playerData.h"
#include"board.h"
#include<array>
#include<functional> 
#include<iostream>
#include<algorithm>
#include<cstdint>
#ifndef EVALUATOR_H
#define EVALUATOR_H

namespace eval{
    // 3 main evaluation techniques
    // position {
    // vague position (where the pieces are) x
    // position aggressiveness 
    
    constexpr std::array<const std::int_fast16_t, 6> PIECEVALUES = {1, 5, 3, 3, 9, 9999};
    constexpr std::array<const std::int_fast16_t, helper::BOARDLENGTH> PPOS = {
        0,   0,   0,  -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,   0,   0,   0,
        0,   0,   0,   3,   10,   3,   3,   3,   3,  10,   3,   0,  0,   0,
        0,   0,  -5,   5,   10,  15,  15,  10,  15,  -5,   5,  -5,  0,   0,
        0,  -10, -10,  10,  20,  30,  30,  20,  30, -10,  10, -10, -10,  0,
       -15, -15, -15,  15,  30,  45,  45,  30,  45, -15,  15, -15, -15, -15,
        10,  10,  10,  20,  40,  60,  60,  40,  60,  20,  20,  10,  10,  10,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,

    };
    constexpr std::array<const std::int_fast16_t, helper::BOARDLENGTH> RPOS = {
        0,   0,   0,  -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,   0,   0,   0,
        0,   0,   0,  -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,   0,   0,   0,
        0,   0,   0,  -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,   0,   0,   0,
        15,  20,  10,  10,  15,  13,  13,  13,  13,  15,  10,  10,  20,  15,
        15,  20,  10,  15,  20,  17,  17,  17,  17,  20,  15,  10,  20,  15,
        15,  20,  10,  13,  17,  10,  10,  10,  10,  17,  13,  10,  20,  15,
        15,  20,  10,  13,  17,  10,  10,  10,  10,  17,  13,  10,  20,  15,
        15,  20,  10,  13,  17,  10,  10,  10,  10,  17,  13,  10,  20,  15,
        15,  20,  10,  13,  17,  10,  10,  10,  10,  17,  13,  10,  20,  15,
        15,  20,  10,  15,  20,  17,  17,  17,  17,  20,  15,  10,  20,  15,
        15,  20,  10,  10,  15,  13,  13,  13,  13,  15,  10,  10,  20,  15,
        0,   0,   0,   10,  10,  10,  10,  10,  10,  10,  10,   0,   0,   0,
        0,   0,   0,   20,  20,  20,  20,  20,  20,  20,  20,   0,   0,   0,
        0,   0,   0,   15,  15,  15,  15,  15,  15,  15,  15,   0,   0,   0,
    };
    constexpr std::array<const std::int_fast16_t, helper::BOARDLENGTH> NPOS = {
        0,   0,   0,  -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,   0,   0,   0,
        0,   0,   0,  -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        15,  15,  13,  11,  2,   2,   2,   2,   2,   2,   11,  13,  15,  15,
        15,  15,  13,  11,  2,   2,   2,   2,   2,   2,   11,  13,  15,  15,
        15,  15,  13,  11,  2,   2,   2,   2,   2,   2,   11,  13,  15,  15,
        15,  15,  13,  11,  2,   2,   2,   2,   2,   2,   11,  13,  15,  15,
        15,  15,  13,  11,  2,   2,   2,   2,   2,   2,   11,  13,  15,  15,
        15,  15,  13,  11,  2,   2,   2,   2,   2,   2,   11,  13,  15,  15,
        15,  15,  13,  11,  2,   2,   2,   2,   2,   2,   11,  13,  15,  15,
        15,  15,  13,  11,  11,  11,  11,  11,  11,  11,  11,  13,  15,  15,
        0,   0,   0,   13,  13,  13,  13,  13,  13,  13,  13,   0,   0,   0,
        0,   0,   0,   15,  15,  15,  15,  15,  15,  15,  15,   0,   0,   0,
        0,   0,   0,   15,  15,  15,  15,  15,  15,  15,  15,   0,   0,   0,


    };
    constexpr std::array<const std::int_fast16_t, helper::BOARDLENGTH> BPOS = {
        0,   0,   0,  -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,   0,   0,   0,
        0,   0,   0,  -5,   10,  10,  10,  10,  10,  10, -5,   0,   0,   0,
        0,   0,   0,   9,   9,   9,   9,   9,   9,   9,   9,   0,   0,   0,
       -15, -8,   9,   10,  10,  10,  10,  10,  10,  10,  10,  9,  -8,  -15,
       -15, -5,   9,   10,  12,  15,  15,  15,  15,  12,  10,  9,  -5,  -15,
       -15, -5,   9,   10,  15,  6,   6,   6,   6,   15,  10,  9,  -5,  -15,
       -15,  1,   9,   10,  15,  6,   6,   6,   6,   15,  10,  9,   1,  -15,
       -15,  1,   9,   10,  15,  6,   6,   6,   6,   15,  10,  9,   1,  -15,
       -15,  1,   9,   10,  15,  6,   6,   6,   6,   15,  10,  9,   1,  -15,
       -15,  0,   9,   10,  12,  15,  15,  15,  15,  12,  10,  9,   0,  -15,
       -15,  0,   9,   10,  10,  10,  10,  10,  10,  10,  10,  9,   0,  -15,
        0,   0,   0,   9,   9,   9,   9,   9,   9,   9,   9,   0,   0,   0,
        0,   0,   0,   0,   0,   1,   1,   1,   0,   0,   0,   0,   0,   0,
        0,   0,   0,  -15, -15, -15, -15, -15, -15, -15, -15,  0,   0,   0,

    };
    constexpr std::array<const std::int_fast16_t, helper::BOARDLENGTH> QPOS = {
        0,   0,   0,  -20, -20, -20, -20, -20, -20, -20, -20,  0,   0,   0,
        0,   0,   0,  -18, -18, -18  -18, -18, -18, -18, -18,   0,   0,   0,
        0,   0,   0,   10,  10,  10,  10,  10,  10,  10,  10,  0,   0,   0,
        22,  20,  20,  12,  15,  13,  13,  13,  13,  15,  12,  20,  20,  22,
        22,  20,  20,  15,  20,  17,  17,  17,  17,  20,  15,  20,  20,  22,
        22,  20,  20,  13,  17,  10,  10,  10,  10,  17,  13,  20,  20,  22,
        22,  20,  20,  13,  17,  10,  10,  10,  10,  17,  13,  20,  20,  22,
        22,  20,  20,  13,  17,  10,  10,  10,  10,  17,  13,  20,  20,  22,
        22,  20,  20,  13,  17,  10,  10,  10,  10,  17,  13,  20,  20,  22,
        22,  20,  20,  15,  20,  17,  17,  17,  17,  20,  15,  20,  20,  22,
        22,  20,  20,  12,  15,  13,  13,  13,  13,  15,  10,  20,  20,  22,
        0,   0,   0,   20,  20,  20,  20,  20,  20,  20,  20,   0,   0,   0,
        0,   0,   0,   20,  20,  20,  20,  20,  20,  20,  20,   0,   0,   0,
        0,   0,   0,   22,  22,  22,  22,  22,  22,  22,  22,   0,   0,   0,

    };
    constexpr std::array<const std::int_fast16_t, helper::BOARDLENGTH> KPOS = {
        0, 0, 0, -3,   -3,   -3,   -3,   -3,   -3,   -3,   -3,   0, 0, 0,
        0, 0, 0, -6,   -6,   -6,   -6,   -6,   -6,   -6,   -6,   0, 0, 0,
        0, 0, 0, -8,   -8,   -8,   -8,   -8,   -8,   -8,   -8,   0, 0, 0,
       -60,  -40,  -20,  -9,  -9,  -9,  -9,  -9,  -9,  -9,  -9,   -20,  -40,  -60,
       -120, -80,  -40,  -18, -18, -18, -18, -18, -18, -18, -18,  -40,  -80,  -120,
       -180, -120, -60,  -27, -27, -27, -27, -27, -27, -27, -27,  -60,  -120, -180,
       -240, -160, -80,  -36, -36, -36, -36, -36, -36, -36, -36,  -80,  -160, -240,
       -300, -200, -100, -45, -45, -45, -45, -45, -45, -45, -45,  -100, -200, -300,
       -360, -240, -120, -54, -54, -54, -54, -54, -54, -54, -54,  -120, -240, -360,
       -420, -280, -140, -63, -63, -63, -63, -63, -63, -63, -63,  -140, -280, -420,
       -480, -320, -160, -72, -72, -72, -72, -72, -72, -72, -72,  -160, -320, -480,
        0, 0, 0, -500, -500, -500, -500, -500, -500, -500, -500, 0, 0, 0,
        0, 0, 0, -550, -550, -550, -550, -550, -550, -550, -550, 0, 0, 0,
        0, 0, 0, -600, -600, -600, -600, -600, -600, -600, -600, 0, 0, 0,
    };
    constexpr std::array<std::reference_wrapper<const std::array<const std::int_fast16_t, helper::BOARDLENGTH>>, 6> POSITIONS = {std::cref(PPOS), std::cref(RPOS), std::cref(NPOS), std::cref(BPOS), std::cref(QPOS), std::cref(KPOS)};

    class Evaluator {
        // P R N B Q K
        // evaluates a single 14x14 index 
        // uses strategy pattern for different piece colours
        constexpr std::int_fast16_t evaluateIndexPosition(types::boardIndex i, types::PieceType t, types::PieceColour c) const {
            // adjust for colour
            unsigned int ci = helper::indexFromColour(c);
            types::boardIndex tmp = helper::rotate90degrees(i, ci);

            const int typeIndex = helper::indexFromType(t);
            assert(typeIndex > -1 && typeIndex < 7);
            return POSITIONS[typeIndex].get()[tmp] * PIECEVALUES[typeIndex];
        }

        constexpr void evaluateMaterial(std::array<std::int_fast16_t, 4> &out, const std::array<const std::reference_wrapper<player::Player>, 4UL> &playersData) const {
            for (unsigned int i = 0; i < playersData.size(); ++i) {
                player::Player &p = playersData[i].get();
                const std::array<const std::reference_wrapper<std::set<types::boardIndex>>, 6UL> pieces = p.getPieces();
                for (unsigned int j = 0; j < pieces.size(); ++j) {
                    out[i] += (pieces.size() * PIECEVALUES[j]);
                }
            }
        }

        constexpr void evaluatePosition(std::array<std::int_fast16_t, 4> &out, const board::Board &board, const std::array<const std::reference_wrapper<player::Player>, 4UL> &playersData) const {
            // we need to evaluate position in correct dimension hence rotations should be considered
            for (unsigned int i = 0; i < playersData.size(); ++i) {
                player::Player &p = playersData[i].get();
                std::array<const std::reference_wrapper<std::set<types::boardIndex>>, 6UL> pieces = p.getPieces();

                for (unsigned int j = 0; j < playersData.size(); ++j) {
                    std::set<types::boardIndex> &pieceSet = pieces[j].get();
                    types::PieceType type = helper::playablePieces[j];

                    std::for_each(pieceSet.cbegin(), pieceSet.cend(), [&type, &out, &i, &p, this](types::boardIndex index) {
                        index = helper::to14BoardIndex(index);
                        out[i] += evaluateIndexPosition(index, type, p.colour());
                    });
                }
            }
        }

        public:
            constexpr std::array<std::int_fast16_t, 4> getEvaluation(const board::Board &board, const std::array<const std::reference_wrapper<player::Player>, 4UL> &playersData) const {
                std::array<std::int_fast16_t, 4> out {0, 0, 0, 0};
                evaluateMaterial(out, playersData);
                evaluatePosition(out, board, playersData);
                return out;
            }


        // std::array<std::int_fast16_t, 4> evaluate 
            // std::array<std::int_fast16_t, 4> getDiffEvaluation(const board::Board &board, const std::array<std::reference_wrapper<player::Player>, 4UL> &playersData) {
            //     const std::array<std::int_fast16_t, 4> evals = getEvaluation(board, playersData);
            //     // find max
            //     unsigned int maxIndex = 0;
            //     for (unsigned int i = 0; i < evals.size(); ++i) {
            //         if (evals[i] > evals[maxIndex]) {
            //             maxIndex = i;
            //         }
            //     }

            //     unsigned int secondIndex = 0;
            //     for (unsigned int i = 0; i < evals.size(); ++i) {
            //         if (i != maxIndex && evals[i] > evals[secondIndex]) {
            //             secondIndex = i;
            //         }
            //     }

            //     std::array<std::int_fast16_t, 4> diff = {0, 0, 0, 0};
            //     for (unsigned int i = 0; i < evals.size(); ++i) {
            //         if (i == maxIndex) {
            //             diff[i] = evals[maxIndex] - evals[secondIndex];
            //         } else {
            //             diff[i] = evals[i] - evals[maxIndex];
            //         }
            //     }


            //     return diff;
            // }      
    };


};

#endif
