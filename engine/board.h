#ifndef BOARD_H
#define BOARD_H
#include"helper.h"
#include "boardDefaults.h"
#include"types.h"
#include"bitboard.h"
#include "move.h"

#include<cstdint>
#include <functional>
#include <ios>
#include<sstream>
#include <stack>
#include <string_view>
#include<utility>
#include<list>


// requires a looping mechanism,
// a size checker (how much of the array is correctly filled)
// a default incorrect value
namespace board {
    // causes namespace pollution
    using std::string_view_literals::operator""sv;

    class Board {
        std::stack<move::Move, std::list<move::Move>> moveStack;
        public: 

            std::vector<move::Move> generatePseudoLegalMoves(types::PieceColour colour) {
                std::vector<move::Move> ret = {};

                const int colourIndex = helper::indexFromColour(colour);
                std::array<bitboard::Bitboard, 6> boards = allPieces[colourIndex]; 
                for (int i = 0; i < boards.size(); i++) {
                    bitboard::Bitboard board = boards[i];
                    types::PieceType type = helper::playablePieces[i];
                    bitboard::Bitboard friendlies = getFriendlies(colour);
                    bitboard::Bitboard blockers = getBlockers(board, colour);
                    bitboard::Bitboard enemies = getEnemies(colour);
                    bitboard::Bitboard shifts = bitboard::Bitboard(boardDefaults::zeroed);
                    bitboard::Bitboard unmovedRooks = getUnmovedRooks(colour);
                    std::pair<int, int> hasCastlingRights = castlingRights[colourIndex];

                    bitboard::Bitboard srcBit = board.popBit();
                    while (!srcBit.empty()) {
                        std::cout << srcBit.empty() << "\n";
                        switch (type) {
                            case types::PieceType::PAWN:
                                shifts = srcBit.genPawnShift(colour, blockers, enemies, friendlies);
                                break;
                            case types::PieceType::KNIGHT:
                                shifts = srcBit.genKnightShift(friendlies);
                                break;
                            case types::PieceType::BISHOP:
                                shifts = srcBit.genBishopShift(blockers, friendlies);
                                break;
                            case types::PieceType::ROOK:
                                shifts = srcBit.genRookShift(blockers, friendlies);
                                break;
                            case types::PieceType::QUEEN:
                                shifts = srcBit.genQueenShift(blockers, friendlies);
                                break;
                            case types::PieceType::KING:
                                shifts = srcBit.genKingShift(hasCastlingRights, colour, blockers, unmovedRooks, friendlies);
                                break;
                            default:
                                // how did we get here?
                                return {};
                        }
                        // keep isolating destBits and forming moves until empty
                        bitboard::Bitboard destBit = shifts.popBit();
                        while (!destBit.empty()) {
                            ret.emplace_back(board, destBit);
                            destBit = shifts.popBit();
                        }
                        srcBit = board.popBit();
                    }

                }
                return ret;
            }

            std::vector<move::Move> generateLegalMoves(types::PieceColour colour) {
                std::vector<move::Move> ret = {};
                std::vector<move::Move> moves = generatePseudoLegalMoves(colour);
                for (move::Move m : moves) {
                    playMove(m);
                    if(!kingInCheck()) {
                        ret.emplace_back(m);
                    }
                    unPlayMove();
                }
                return ret;
            }

            // plays a move
            void playMove(move::Move) {
            }

            // unplays a move
            void unPlayMove() {
                move::Move move = moveStack.top();
                moveStack.pop();
            }

            // checks whether the king is in check
            int kingInCheck(types::PieceColour colour) {
                return 0;
            }

            std::array<bitboard::Bitboard, 6> redPieces = {
                boardDefaults::redPawns,
                boardDefaults::redRooks,
                boardDefaults::redKnights,
                boardDefaults::redBishops,
                boardDefaults::redQueens,
                boardDefaults::redKings
            };
            std::array<bitboard::Bitboard, 6> bluePieces = {
                boardDefaults::bluePawns,
                boardDefaults::blueRooks,
                boardDefaults::blueKnights,
                boardDefaults::blueBishops,
                boardDefaults::blueQueens,
                boardDefaults::blueKings
            };
            std::array<bitboard::Bitboard, 6> greenPieces = {
                boardDefaults::greenPawns,
                boardDefaults::greenRooks,
                boardDefaults::greenKnights,
                boardDefaults::greenBishops,
                boardDefaults::greenQueens,
                boardDefaults::greenKings
            };
            std::array<bitboard::Bitboard, 6> yellowPieces = {
                boardDefaults::yellowPawns,
                boardDefaults::yellowRooks,
                boardDefaults::yellowKnights,
                boardDefaults::yellowBishops,
                boardDefaults::yellowQueens,
                boardDefaults::yellowKings,
            };

            std::array<std::reference_wrapper<std::array<bitboard::Bitboard, 6>>, 4> allPieces = {std::ref(redPieces), std::ref(bluePieces), std::ref(greenPieces), std::ref(yellowPieces)};
            std::array<bitboard::Bitboard, 4> pieceSums = {
                boardDefaults::redPieces,
                boardDefaults::bluePieces,
                boardDefaults::yellowPieces,
                boardDefaults::greenPieces
            };

            std::array<std::pair<int, int>, 4> castlingRights = {
                std::pair(1, 1),
                std::pair(1, 1),
                std::pair(1, 1),
                std::pair(1, 1)
            };

            bitboard::Bitboard getFriendlies(types::PieceColour colour) {
                int index = helper::indexFromColour(colour);
                return pieceSums[index];
            }

            bitboard::Bitboard getBlockers(bitboard::Bitboard board, types::PieceColour col) {
                const int colIndex = helper::indexFromColour(col);
                bitboard::Bitboard ret = boardDefaults::zeroed;
                for (int i = 0; i < pieceSums.size(); i++) {
                    if (i == colIndex) {
                        ret = ret.combine(pieceSums[i].subtract(board));
                    } else {
                        ret = ret.combine(pieceSums[i]);
                    }
                }
                return ret;
            }

            bitboard::Bitboard getEnemies(types::PieceColour col) {
                const int colIndex = helper::indexFromColour(col);
                bitboard::Bitboard ret = boardDefaults::zeroed;
                for (int i = 0; i < pieceSums.size(); i++) {
                    if (i != colIndex) {
                        ret = ret.combine(pieceSums[i]);
                    }
                }
                return ret;
            }

            bitboard::Bitboard getUnmovedRooks(types::PieceColour col) {
                const int colourIndex = helper::indexFromColour(col);
                const int rookIndex = helper::indexFromType(types::PieceType::ROOK);
                bitboard::Bitboard rooks = allPieces[colourIndex].get()[rookIndex];
                bitboard::Bitboard defaultRooks = boardDefaults::zeroed;
                switch(col) {
                    case types::PieceColour::RED:
                        defaultRooks = boardDefaults::redRooks;
                        break;
                    case types::PieceColour::BLUE:
                        defaultRooks = boardDefaults::blueRooks;
                        break;
                    case types::PieceColour::YELLOW:
                        defaultRooks = boardDefaults::yellowRooks;
                        break;
                    case types::PieceColour::GREEN:
                        defaultRooks = boardDefaults::greenRooks;
                        break;
                    default:
                        break;
                }
                return rooks.intersect(defaultRooks);
            }
    };
};

#endif
