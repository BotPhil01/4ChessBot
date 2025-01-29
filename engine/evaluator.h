#include"types.h"
#include"helper.h"
#include"playerData.h"
#include"board.h"
#include<array>
#include<functional> 
#include<iostream>
#include<algorithm>
#ifndef EVALUATOR_H
#define EVALUATOR_H

namespace eval{
    // 3 main evaluation techniques
    // position {
    // vague position (where the pieces are) x
    // position aggressiveness 
    
    constexpr std::array<float, 6> PIECEVALUES = {1.0f, 5.0f, 2.8f, 3.0f, 9.0f, 9999.0f};
    constexpr std::array<float, helper::BOARDLENGTH> PPOS = {
        0.0f, 0.0f, 0.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 3.0f, 10.0f, 3.0f, 3.0f, 3.0f, 3.0f, 10.0f, 3.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, -5.0f, 5.0f, 10.0f, 15.0f, 15.0f, 10.0f, 15.0f, -5.0f, 5.0f, -5.0f, 0.0f, 0.0f,
        0.0f, -10.0f, -10.0f, 10.0f, 20.0f, 30.0f, 30.0f, 20.0f, 30.0f, -10.0f, 10.0f, -10.0f, -10.0f, 0.0f,
        -15.0f, -15.0f, -15.0f, 15.0f, 30.0f, 45.0f, 45.0f, 30.0f, 45.0f, -15.0f, 15.0f, -15.0f, -15.0f, -15.0f,
        10.0f, 10.0f, 10.0f, 20.0f, 40.0f, 60.0f, 60.0f, 40.0f, 60.0f, 20.0f, 20.0f, 10.0f, 10.0f, 10.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    };
    constexpr std::array<float, helper::BOARDLENGTH> RPOS = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        0.0f, 0.0f, 0.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, 0.0f, 0.0f, 0.0f,
    };
    constexpr std::array<float, helper::BOARDLENGTH> NPOS = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        0.0f, 0.0f, 0.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, 0.0f, 0.0f, 0.0f,

    };
    constexpr std::array<float, helper::BOARDLENGTH> BPOS = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        0.0f, 0.0f, 0.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, 0.0f, 0.0f, 0.0f,
    };
    constexpr std::array<float, helper::BOARDLENGTH> QPOS = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f,
        0.0f, 0.0f, 0.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, -30.0f, 0.0f, 0.0f, 0.0f,

    };
    constexpr std::array<float, helper::BOARDLENGTH> KPOS = {
        0.0f, 0.0f, 0.0f, -3.0f,   -3.0f,   -3.0f,   -3.0f,   -3.0f,   -3.0f,   -3.0f,   -3.0f,   0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -6.0f,   -6.0f,   -6.0f,   -6.0f,   -6.0f,   -6.0f,   -6.0f,   -6.0f,   0.0f, 0.0f, 0.0f,
        -60.0f,    -40.0f,    -20.0f,    -9.0f,   -9.0f,   -9.0f,   -9.0f,   -9.0f,   -9.0f,   -9.0f,   -9.0f,   -20.0f,    -40.0f,    -60.0f,
        -120.0f,   -80.0f,    -40.0f,    -18.0f,  -18.0f,  -18.0f,  -18.0f,  -18.0f,  -18.0f,  -18.0f,  -18.0f,  -40.0f,    -80.0f,    -120.0f,
        -180.0f,   -120.0f,   -60.0f,    -27.0f,  -27.0f,  -27.0f,  -27.0f,  -27.0f,  -27.0f,  -27.0f,  -27.0f,  -60.0f,    -120.0f,   -180.0f,
        -240.0f,   -160.0f,   -80.0f,    -36.0f,  -36.0f,  -36.0f,  -36.0f,  -36.0f,  -36.0f,  -36.0f,  -36.0f,  -80.0f,    -160.0f,   -240.0f,
        -300.0f,   -200.0f,   -100.0f,   -45.0f,  -45.0f,  -45.0f,  -45.0f,  -45.0f,  -45.0f,  -45.0f,  -45.0f,  -100.0f,   -200.0f,   -300.0f,
        -360.0f,   -240.0f,   -120.0f,   -54.0f,  -54.0f,  -54.0f,  -54.0f,  -54.0f,  -54.0f,  -54.0f,  -54.0f,  -120.0f,   -240.0f,   -360.0f,
        -420.0f,   -280.0f,   -140.0f,   -63.0f,  -63.0f,  -63.0f,  -63.0f,  -63.0f,  -63.0f,  -63.0f,  -63.0f,  -140.0f,   -280.0f,   -420.0f,
        -480.0f,   -320.0f,   -160.0f,   -72.0f,  -72.0f,  -72.0f,  -72.0f,  -72.0f,  -72.0f,  -72.0f,  -72.0f,  -160.0f,   -320.0f,   -480.0f,
        0.0f, 0.0f, 0.0f, -500.0f, -500.0f, -500.0f, -500.0f, -500.0f, -500.0f, -500.0f, -500.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -550.0f, -550.0f, -550.0f, -550.0f, -550.0f, -550.0f, -550.0f, -550.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -600.0f, -600.0f, -600.0f, -600.0f, -600.0f, -600.0f, -600.0f, -600.0f, 0.0f, 0.0f, 0.0f,
    };
    constexpr std::array<std::reference_wrapper<const std::array<float, helper::BOARDLENGTH>>, 6> POSITIONS = {std::cref(PPOS), std::cref(RPOS), std::cref(NPOS), std::cref(BPOS), std::cref(QPOS), std::cref(KPOS)};

    class Evaluator {
        // P R N B Q K
        // evaluates a single 14x14 index 
        // uses strategy pattern for different piece colours
        float evaluateIndexPosition(types::boardIndex i, types::PieceType t, types::PieceColour c) {
            // adjust for colour
            unsigned int ci = helper::indexFromColour(c);
            types::boardIndex tmp = helper::rotate90degrees(i, ci);

            const int typeIndex = helper::indexFromType(t);
            assert(typeIndex > -1 && typeIndex < 7);
            return POSITIONS[typeIndex].get()[tmp] * PIECEVALUES[typeIndex];
        }

        void evaluateMaterial(std::array<float, 4> &out, const std::array<std::reference_wrapper<player::Player>, 4UL> &playersData) {
            for (unsigned int i = 0; i < playersData.size(); ++i) {
                player::Player p = playersData[i].get();
                const std::vector<std::reference_wrapper<std::set<types::boardIndex>>> pieces = p.getPieces();
                for (unsigned int j = 0; j < pieces.size(); ++j) {
                    out[i] += (pieces.size() * PIECEVALUES[j]);
                }
            }
        }

        void evaluatePosition(std::array<float, 4> &out, const board::Board &board, const std::array<std::reference_wrapper<player::Player>, 4UL> &playersData) {
            // we need to evaluate position in correct dimension hence rotations should be considered
            for (unsigned int i = 0; i < playersData.size(); ++i) {
                player::Player p = playersData[i].get();
                std::vector<std::reference_wrapper<std::set<types::boardIndex>>> pieces = p.getPieces();

                for (unsigned int j = 0; j < playersData.size(); ++j) {
                    const std::set<types::boardIndex> pieceSet = pieces[j].get();
                    types::PieceType type = helper::playablePieces[j];

                    std::for_each(pieceSet.cbegin(), pieceSet.cend(), [&type, &out, &i, &p, this](types::boardIndex index) {
                        index = helper::to14BoardIndex(index);
                        out[i] += evaluateIndexPosition(index, type, p.colour());
                    });
                }
            }
        }

        public:
            std::array<float, 4> getEvaluation(const board::Board &board, const std::array<std::reference_wrapper<player::Player>, 4UL> &playersData) {
                std::array<float, 4> out {0.0F, 0.0F, 0.0F, 0.0F};
                evaluateMaterial(out, playersData);
                evaluatePosition(out, board, playersData);
                return out;
            }


        // std::array<float, 4> evaluate 
            std::array<float, 4> getDiffEvaluation(const board::Board &board, const std::array<std::reference_wrapper<player::Player>, 4UL> &playersData) {
                const std::array<float, 4> evals = getEvaluation(board, playersData);
                // find max
                unsigned int maxIndex = 0;
                for (unsigned int i = 0; i < evals.size(); ++i) {
                    if (evals[i] > evals[maxIndex]) {
                        maxIndex = i;
                    }
                }

                unsigned int secondIndex = 0;
                for (unsigned int i = 0; i < evals.size(); ++i) {
                    if (i != maxIndex && evals[i] > evals[secondIndex]) {
                        secondIndex = i;
                    }
                }

                std::array<float, 4> diff = {0.0f, 0.0f, 0.0f, 0.0f};
                for (unsigned int i = 0; i < evals.size(); ++i) {
                    if (i == maxIndex) {
                        diff[i] = evals[maxIndex] - evals[secondIndex];
                    } else {
                        diff[i] = evals[i] - evals[maxIndex];
                    }
                }


                return diff;
            }      
    };


};

#endif
