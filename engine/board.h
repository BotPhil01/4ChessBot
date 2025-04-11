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
        types::PieceColour currentTurn = helper::RED;
        std::array<bitboard::Bitboard, 6> redPieces;
        std::array<bitboard::Bitboard, 6> bluePieces;
        std::array<bitboard::Bitboard, 6> yellowPieces;
        std::array<bitboard::Bitboard, 6> greenPieces;
        std::array<std::reference_wrapper<std::array<bitboard::Bitboard, 6>>, 4> allPieces;
        std::array<bitboard::Bitboard, 4> pieceSums;
        std::array<std::pair<int, int>, 4> castlingRights = {
            std::pair(1, 1),
            std::pair(1, 1),
            std::pair(1, 1),
            std::pair(1, 1)
        };
        std::stack<move::Move, std::list<move::Move>> moveStack;

        std::array<bitboard::Bitboard, 4> initSums() {
            std::array<bitboard::Bitboard, 4> ret;
            for (int i = 0; i < 4; i++) {
                bitboard::Bitboard tmp = boardDefaults::zeroed;
                for (int j = 0; j < 6; j++) {
                    tmp = tmp.combine(allPieces[i].get()[j]);
                }
                ret[i] = tmp;
            }
            return ret;
        }

        std::array<int, 4> playerCheckmate = {0, 0, 0, 0};

        public: 

        Board(bitboard::Bitboard b, types::PieceType type) :
            redPieces({
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed
                    }),
            bluePieces({
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed
                    }),
            yellowPieces({
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed
                    }),
            greenPieces({
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed, 
                    boardDefaults::zeroed
                    }),
            allPieces({
                    std::ref(redPieces), std::ref(bluePieces), 
                    std::ref(yellowPieces), std::ref(greenPieces)
                    }),
            pieceSums(initSums())
            {
                int index = helper::indexFromType(type);
                redPieces[index] = b;
                pieceSums = initSums();
            }

        Board() :
            redPieces({
                    boardDefaults::redPawns,
                    boardDefaults::redRooks,
                    boardDefaults::redKnights,
                    boardDefaults::redBishops,
                    boardDefaults::redQueens,
                    boardDefaults::redKings
                    }),
            bluePieces({
                    boardDefaults::bluePawns,
                    boardDefaults::blueRooks,
                    boardDefaults::blueKnights,
                    boardDefaults::blueBishops,
                    boardDefaults::blueQueens,
                    boardDefaults::blueKings
                    }),
            yellowPieces({
                    boardDefaults::yellowPawns,
                    boardDefaults::yellowRooks,
                    boardDefaults::yellowKnights,
                    boardDefaults::yellowBishops,
                    boardDefaults::yellowQueens,
                    boardDefaults::yellowKings,
                    }),
            greenPieces({ 
                    boardDefaults::greenPawns,
                    boardDefaults::greenRooks,
                    boardDefaults::greenKnights,
                    boardDefaults::greenBishops,
                    boardDefaults::greenQueens,
                    boardDefaults::greenKings
                    }),
            allPieces({
                    std::ref(redPieces), std::ref(bluePieces), 
                    std::ref(yellowPieces), std::ref(greenPieces)
                    }),
            pieceSums(initSums())
        {
            asserts();
        }

        void asserts() {
            for (int i = 0; i < allPieces.size(); i++) {
                std::array<bitboard::Bitboard, 6> boards = allPieces[i].get();
                for (bitboard::Bitboard b : boards) {
                    b.boundsCheck();
                }
            }
            for (int i = 0; i <pieceSums.size(); i++) {
                bitboard::Bitboard b = pieceSums[i];
                b.boundsCheck();
            }
        }

        types::PieceColour getCurrentTurn() {
            return currentTurn;
        }

        move::Move indicesToMove(std::pair<int, int> from, std::pair<int, int> to) {
            std::cout << allPieces[0].get()[0].boolBoundsCheck();
            bitboard::Bitboard unaryFrom(from);
            bitboard::Bitboard unaryTo(from);

            types::PieceColour colour;
            // scan through all the players and search for the bits in the associated from board
            for (int i = 0; i < helper::playableColours.size(); i++) {
                bitboard::Bitboard sum = pieceSums[i];
                if (sum.intersect(unaryFrom).unary()) {
                    colour = helper::playableColours[i];
                    break;
                }
            }
            const int colourIndex = helper::indexFromColour(colour);

            bitboard::Bitboard &sum = pieceSums[colourIndex];
            std::array<bitboard::Bitboard, 6> &boards = allPieces[colourIndex];
            for (int i = 0; i < helper::playablePieces.size(); i++) {
                bitboard::Bitboard &board = boards[i];
                if (unaryFrom.intersect(board).unary()) {
                    // found the board
                    return move::Move(sum, board, unaryFrom, unaryTo);
                }
            }
            assert(false);
        }

        std::array<int, 6> getPlayerCounts(const types::PieceColour colour) const {
            const int index = helper::indexFromColour(colour);
            const std::array<bitboard::Bitboard, 6> boards = allPieces[index];

            std::array<int, 6> ret = {};
            for (int i = 0; i < 6; i++) {
                ret[i] = boards[i].count();
            }
            return ret;
        }
            


        void setPlayerCheckmate(types::PieceColour col) {
            const int index = helper::indexFromColour(col);
            playerCheckmate[index] = 1;
        }

        void checkStarts() {
            // check piece sums
            for (int i = 0; i < 4; i++) {
                types::PieceColour col = helper::playableColours[i];
                bitboard::Bitboard tmp = boardDefaults::zeroed;
                for (int j = 0; j < 6; j++) {
                    tmp = tmp.combine(allPieces[i].get()[j]);
                }
                assert(tmp.equals(pieceSums[i]));
            }
            bitboard::Bitboard tmp = boardDefaults::zeroed;
            // check the overlap
            for (int i = 0; i < 4; i++) {
                bitboard::Bitboard inner = boardDefaults::zeroed;
                tmp = tmp.intersect(pieceSums[i]);
                for (int j = 0; j < 6; j++) {
                    inner = inner.intersect(allPieces[i].get()[j]);
                }
                assert(inner.equals(boardDefaults::zeroed));
            }
            assert(tmp.equals(boardDefaults::zeroed));

        }

        void fillBoardStrs(
                std::array<std::array<std::string, 4 * (COLS)>, 4 * (ROWS)> &strGrid,
                bitboard::Bitboard board,
                const types::PieceColour colour,
                const types::PieceType type
                ) {
            if (!board.empty()) {
                std::string str = "";
                str += helper::colourToChar(colour);
                str += helper::typeToChar(type);

                bitboard::Bitboard bit =  board.popBit();
                while (!bit.empty()) {
                    bitboard::coords coords = bit.bitIndex();
                    const int col = coords.outerX * 4 + coords.innerX;
                    const int row = coords.outerY * 4 + coords.innerY;
                    strGrid[row][col] = str;
                    bit = board.popBit();
                }
            }
        }

        void populateEmpty(
                std::array<std::array<std::string, 4 * (ROWS)>, 4 * (COLS)> &strGrid
                ) {
            for (int i = 1; i < strGrid.size() - 1; i++) {
                for (int j = 1; j < strGrid[0].size(); j++) {
                    strGrid[i][j] = "##"sv;
                }
            }

            for (int i = 0; i < strGrid.size(); i++) {
                if (i == 0 || i == strGrid.size() - 1) {
                    for (int j = 0; j < strGrid[0].size(); j++) {
                        strGrid[i][j] = "XX"sv;
                    }
                } else if (i > 3 && i < 12) {
                    strGrid[i][0] = "XX"sv;
                    strGrid[i][strGrid.size()-1] = "XX"sv;
                } else {
                    for (int j = 0; j < 4; j++) {
                        strGrid[i][0 + j] = "XX"sv;
                        strGrid[i][strGrid.size() - 1 - j] = "XX"sv;
                    }
                }
            }
        }

        // uses allPieces
        void print() {
            using innerType = std::array<std::string, 4 * (ROWS)>;
            using outerType = std::array<innerType, 4 * (COLS)>;
            outerType strGrid;
            populateEmpty(strGrid);
            for (types::PieceColour col : helper::playableColours) {
                const int colIndex = helper::indexFromColour(col);
                for (types::PieceType typ : helper::playablePieces) {
                    const int typIndex = helper::indexFromType(typ);
                    const bitboard::Bitboard board = allPieces[colIndex].get()[typIndex];
                    fillBoardStrs(strGrid, board, col, typ);
                }
            }
            
            for (innerType row : strGrid) {
                for (std::string str : row) {
                    std::cout << str << " ";
                }
                std::cout << "\n";
            }
            std::cout.flush();
        }

        std::pair<types::PieceColour, types::PieceType> getToPiece(const bitboard::Bitboard targetBit) {
            for (int i = 0; i < allPieces.size(); i++) {
                const std::reference_wrapper<std::array<bitboard::Bitboard, 6>> pieces = allPieces[i];
                const std::array<bitboard::Bitboard, 6> boards = pieces.get();
                for (int j = 0; j < boards.size(); j++) {
                    bitboard::Bitboard board = boards[j];
                    if (!board.intersect(targetBit).empty()) {
                        return std::pair(helper::playableColours[i], helper::playablePieces[j]);
                    }
                }
            }
            return std::pair(types::PieceColour::NONE, types::PieceType::EMPTY);
        }

        // produces bad boards
        std::vector<move::Move> generatePseudoLegalMoves(types::PieceColour colour) {
            std::vector<move::Move> ret = {};

            const int colourIndex = helper::indexFromColour(colour);
            bitboard::Bitboard &sumBoard = pieceSums[colourIndex];
            std::array<bitboard::Bitboard, 6> &boards = allPieces[colourIndex]; 
            for (int i = 0; i < boards.size(); i++) {
                bitboard::Bitboard &board = boards[i];
                bitboard::Bitboard tmp = board;
                types::PieceType type = helper::playablePieces[i];
                bitboard::Bitboard friendlies = getFriendlies(colour);
                bitboard::Bitboard blockers = getBlockers(tmp, colour);
                bitboard::Bitboard enemies = getEnemies(colour);
                bitboard::Bitboard shifts = bitboard::Bitboard(boardDefaults::zeroed);
                bitboard::Bitboard unmovedRooks = getUnmovedRooks(colour);
                std::pair<int, int> hasCastlingRights = castlingRights[colourIndex];

                // srcbit is incorrect heere
                bitboard::Bitboard srcBit = tmp.popBit();
                while (!srcBit.empty()) {
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
                        std::pair<types::PieceColour, types::PieceType> toInfo = getToPiece(destBit);
                        ret.emplace_back(sumBoard, board, srcBit, destBit,
                                destBit, colour, toInfo.first, type, toInfo.second);
                        destBit = shifts.popBit();
                    }
                    srcBit = tmp.popBit();
                }
            }
            return ret;
        }

        std::vector<move::Move> generateLegalMoves(types::PieceColour colour) {
            std::vector<move::Move> ret = {};
            std::vector<move::Move> moves = generatePseudoLegalMoves(colour);
            for (int i = 0; i < moves.size(); i++) {
                move::Move m = moves[i];
                playMove(m);
                if(!kingInCheck(colour)) {
                    ret.emplace_back(m);
                }
                unPlayMove();
            }
            return ret;
        }

        void incrementCurrentTurn() {
            currentTurn = helper::playableColours[(helper::indexFromColour(currentTurn) + 1) % 4];
        }
        // plays a move
        // TODO expand for special moves
        void playMove(move::Move m) {
            assert(m.unarySrcBoard.unary());
            assert(m.destBoard.unary());
            m.srcBoard = m.srcBoard.subtract(m.unarySrcBoard).combine(m.destBoard);
            m.srcBoard.boundsCheck();
            m.sumBoard = m.sumBoard.subtract(m.unarySrcBoard).combine(m.destBoard);
            m.sumBoard.boundsCheck();
            moveStack.push(m);
            incrementCurrentTurn();
        }

        void decrementCurrentTurn() {
            int index = (helper::indexFromColour(currentTurn) - 1);
            if (index < 0) { 
                index = 3;
            }

            currentTurn = helper::playableColours[index];
        }

        // unplays a move
        // TODO expand for special moves
        void unPlayMove() {
            move::Move m = moveStack.top();
            moveStack.pop();
            // srcboard is shared between the threads leading to corruption
            m.srcBoard = m.srcBoard.subtract(m.destBoard).combine(m.unarySrcBoard);
            m.srcBoard.boundsCheck();
            m.sumBoard = m.sumBoard.subtract(m.destBoard).combine(m.unarySrcBoard);
            m.sumBoard.boundsCheck();
            decrementCurrentTurn();
        }

        // checks whether the king is in check
        int kingInCheck(types::PieceColour colour) {
            // get king board for comparison
            const int colourIndex = helper::indexFromColour(colour);
            const int kingIndex = helper::indexFromType(types::PieceType::KING);
            const bitboard::Bitboard kingBoard = allPieces[colourIndex].get()[kingIndex];

            for (types::PieceColour enemyColour : helper::playableColours) {
                // skip generating ourselves
                if (colour == enemyColour) {
                    continue;
                }

                // generate the moves
                const std::vector<move::Move> moves = generatePseudoLegalMoves(enemyColour);
                for (move::Move m : moves) {
                    if (!m.destBoard.intersect(kingBoard).empty()) {
                        return 1;
                    }
                }
            }
            return 0;
        }

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
