#include"types.h"
#include"helper.h"
#include<vector>
using namespace helper;
using namespace types;
#ifndef BOARD_H
#define BOARD_H

namespace board {
    // board is padded to 16x18
    class Board {
        private:
            std::vector<Square> boardVector;
            const char PADDEDCOLS = 16;
            const char PADDEDROWS = 18;
            PieceColour turn;
            std::vector<Move> moveVector;
        public:

            Board() {
                // construct to initial position
                for (short i = 0; i < 288; ++i) {
                    boardVector.emplace_back(generateSquare(i));
                }
                turn = initailTurn;
                moveVector = std::vector<Move> ();
            }

            boardIndex toIndex(unsigned char r, unsigned char c) {
                return (r * PADDEDCOLS + c);
            }

            Square & getSquare(boardIndex r, boardIndex c) { // from 14x14
                short a1 = 33;
                return boardVector.at(r * PADDEDCOLS + (c + 1) + a1);
            }

            // Square & getSquareByType(COLUMN c, short r) {}
            Square & getSquarePadded(boardIndex r, boardIndex c) { // from 16x18
                return boardVector.at(r * 16 + c);
            }

            void printBoard() {
                // std::size_t length = boardVector.size();
                // algo is weird but prints correctly according to red
                // orientation in order NESW = YGRB
                for (short i = PADDEDROWS - 1; i >= 0; --i) {
                    for (int j = 0; j < PADDEDCOLS; ++j) {
                        PieceType type = boardVector[16 * i + j].type;
                        std::cout << PieceTypeToString(type).substr(0, 2) << " ";
                    }
                    std::cout << '\n';
                }
            }

            // helper function to query board if the piece at s can capture at t also checking if t exists (can be anything on board as long as diff colours)
            // note this is pseudo legal and hence should not be used as a definitive answer
            // assume src is a valid piece ie not empty
            // checks if target is on the board
            bool existsCapturable(boardIndex src, boardIndex trgt) {
                Square const trgtSqu = boardVector.at(trgt);
                Square const srcSqu = boardVector.at(src);
                // std::cout << "target colour for " << trgt << ": " << PieceCOlourToString(trgtSqu.pieceColour) << "\n";
                // std::cout << "src colour for " << src << ": " << PieceCOlourToString(srcSqu.pieceColour) << "\n";
                return isOnBoard(trgt) && trgtSqu.pieceColour != srcSqu.pieceColour && trgtSqu.type != PieceType::BLOCK;
            }

            // STRICTER THAN existsCapturable
            bool existsEnemyPiece(boardIndex src, boardIndex trgt) {
                Square const trgtSqu = boardVector.at(trgt);
                Square const srcSqu = boardVector.at(src);
                return isOnBoard(trgt) && trgtSqu.pieceColour != srcSqu.pieceColour && trgtSqu.type != PieceType::BLOCK && trgtSqu.type != PieceType::EMPTY;
            }

            std::vector<Move> bulkCreateMove(boardIndex src, std::vector<boardIndex> trgts) {
                std::vector<Move> out {};
                Move m = createMove(src, 37);
                for (boardIndex t : trgts) {
                    m = createMove(src, t);
                    out.emplace_back(m.fromSquare, m.fromIndex, m.toSquare, m.toIndex);
                }
                return out;
            }

            Move createMove(boardIndex src, boardIndex trgt) {
                Square srcSquare = boardVector[src];
                Square trgtSquare = boardVector[trgt];
                return Move(srcSquare, src, trgtSquare, trgt);
            }

            std::vector<Move> generatePseudoLegalMoves() {
                std::size_t size = boardVector.size();
                std::vector<Move> out {};
                // std::cout << "out size after initialisation: " << out.size() << "\n";
                bool debug = false;
                // std::cout << "generating pseudo legal moves\n";
                for (int i = 0; i < size; i++) {
                    if (boardVector[i].pieceColour == turn) {
                        switch(boardVector[i].type) {
                            case PieceType::PAWN:
                                if (!debug) {
                                    std::cout << "index of pawn: " << i << "\n";
                                    std::cout << "bulkCreateMove[0] after i: " << PieceTypeToString(bulkCreateMove(i, pawnShift(i))[0].fromSquare.type) << "\n";
                                    
                                    debug = true;
                                }
                                out = concat(out, bulkCreateMove(i, pawnShift(i)));
                                
                                break;
                            case PieceType::ROOK:
                                out = concat(out, bulkCreateMove(i, rookShift(i)));
                                break;
                            case PieceType::KNIGHT:
                                out = concat(out, bulkCreateMove(i, knightShift(i)));
                                break;
                            case PieceType::BISHOP:
                                out = concat(out, bulkCreateMove(i, bishopShift(i)));
                                break;
                            case PieceType::KING:
                                out = concat(out, bulkCreateMove(i, kingShift(i)));
                                break;
                            case PieceType::QUEEN:
                                out = concat(out, bulkCreateMove(i, queenShift(i)));
                                break;
                            default:
                                continue;
                        }
                    }
                    // if (!debug && out.size() > 0) {
                    //     debug = true;
                    //     std::cout << "first i to print to out: " << i << "\n";
                    //     std::cout << "First elem after i printing: " << out[0].fromSquare.toStringView() << "\n";
                    // }
                }
                
                std::cout << "out[0]: " << moveToString(out[0]) << "\n";
                std::cout << "out[0] with type: " << PieceTypeToString(out[0].fromSquare.type) << "\n";
                return out;
            }

            // asks whether c's king is in check in current board position
            bool isKingInCheck(PieceColour c) {
                // idea
                // get king index
                boardIndex kingIndex = 300;
                unsigned short size = boardVector.size();
                for (unsigned short i = 0; i < size; ++i) {
                    Square s = boardVector[i];
                    if (s.pieceColour == c && s.type == PieceType::KING) {
                        kingIndex = i;
                        break;
                    }
                }


                // generate possible attacks

                // check ray attacks () and 
                std::vector<Direction> diags {Direction::NORTHEAST, Direction::SOUTHEAST, Direction::SOUTHWEST, Direction::NORTHWEST};
                std::vector<Direction> upDowns {Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST};
                for (Direction d : diags) {
                    // sliding piece check
                    auto ray = getRay(kingIndex, d);
                    if (ray.size() > 0) {
                        auto last = ray.back();
                        if (existsEnemyPiece(kingIndex, last) && (boardVector.at(last).type == PieceType::BISHOP || boardVector.at(last).type == PieceType::QUEEN)) {
                            return true;
                        }
                    }

                    // pawn check
                    auto diagIndex = shiftOne(kingIndex, d);
                    if (existsEnemyPiece(kingIndex, diagIndex) && (boardVector.at(diagIndex).type == PieceType::PAWN)) {
                        // get the pawn shift
                        // if the result of the pawn shift is the kignIndex then return true 
                        auto pawnAttacks = pawnCaptureShift(diagIndex);
                        for (auto attack : pawnAttacks) {
                            if (attack == kingIndex) {
                                return true;
                            }
                        }
                    }
                }

                for (Direction d : upDowns) {
                    auto ray = getRay(kingIndex, d);
                    if (ray.size() > 0) {
                        auto last = ray.back();
                        if (existsEnemyPiece(kingIndex, last) && (boardVector.at(last).type == PieceType::ROOK || boardVector.at(last).type == PieceType::QUEEN)) {
                            return true;
                        }
                    }
                }

                // check knight
                auto knightRays = knightShift(kingIndex);
                for (boardIndex i : knightRays) {
                    if (existsEnemyPiece(kingIndex, i) && boardVector.at(i).type == PieceType::KNIGHT) {
                        return true;
                    }
                }
                return false;
            }

            std::vector<Move> generateLegalMoves() {
                std::vector<Move> out;
                auto moves = generatePseudoLegalMoves();
                std::cout << "moves generated\n";
                for (auto m : moves) {
                    playMove(m);
                    if (!isKingInCheck(turn)) {
                        out.emplace_back(m);
                    }
                    unPlayMove();
                }
                return out;
            }

            boardIndex shiftOne(boardIndex i, Direction d) {
                switch (d) {
                    case Direction::NORTH: 
                        return i + 16;
                    case Direction::NORTHEAST: 
                        return i + 17;
                    case Direction::EAST:
                        return i + 1;
                    case Direction::SOUTHEAST:
                        return i - 15;
                    case Direction::SOUTH:
                        return i - 16;
                    case Direction::SOUTHWEST:
                        return i - 17;
                    case Direction::WEST:
                        return i - 1;
                    case Direction::NORTHWEST:
                        return i + 15;
                    default:
                        return i;
                };
            }

            // get colour's last move 
            // returns valid iterator if move exists otherwise returns .end() iterator
            std::vector<Move>::iterator getLastMoveIterator(PieceColour colour) {
                const size_t vs = moveVector.size();
                unsigned char min = std::min(vs, (const size_t) 4); 
                std::vector<Move>::iterator iterator = moveVector.end() - 1;
                for (int i = 0; i < min; ++i) {
                    Move m = moveVector[vs - i]; 
                    if (m.fromSquare.pieceColour == colour) {
                        return iterator - i;
                    }
                }
                return moveVector.end();
            }

            // check if a square is empty by index
            bool isEmpty(boardIndex i) {
                return boardVector[i].type == PieceType::EMPTY;
            }

            bool isOnBoard(boardIndex i) {
                for (boardIndex c : cornerIndices) { // not in corners
                    if (c == i) {
                        return false;
                    }
                }
                boardIndex rem = i % 16;
                return rem != 0 && rem != 15 && i < 252 && i > 35;
            }

        
            std::vector<boardIndex> pawnShift(boardIndex index) {
                auto out = concat(pawnQuietShift(index), pawnCaptureShift(index));
                return out;
            }

            // Quiet move shift checks if on the board
            std::vector<boardIndex> pawnQuietShift(boardIndex index) {
                // std::cout << "quiet shifting\n";
                std::vector<boardIndex> out = {};
                boardIndex m = shiftOne(index, Direction::NORTH);
                if (isEmpty(m) && isOnBoard(m)) {out.emplace_back(m);}
                if (!isPawnStart(index)) {
                    return out;
                }
                m = shiftOne(m, Direction::NORTH);
                if (isEmpty(m) && isOnBoard(m)) {out.emplace_back(m);}
                // std::cout << "out size: "<< out.size() << "\n";
                return out;
            }

            // includes en peasant
            std::vector<boardIndex> pawnCaptureShift(boardIndex index) {
                // check if there exists a capturable piece beside 
                // TODO optimise later
                std::vector<boardIndex> out = std::vector<boardIndex> {}; // en peasant is unlikely 
                // regular captures
                boardIndex target = shiftOne(index, getUpRight(boardVector[index].pieceColour));
                bool upRightEmpty = isEmpty(target);
                if (existsEnemyPiece(index, target)) {
                    out.emplace_back(target);
                }

                target = shiftOne(index, getUpLeft(boardVector[index].pieceColour)); // go to next possible capture
                bool upLeftEmpty = isEmpty(target);
                if (existsEnemyPiece(index, target)) {
                    out.emplace_back(target);
                }

                // en peasant
                target = shiftOne(index, getLeft(boardVector[index].pieceColour));
                PieceColour targetColour = boardVector[target].pieceColour; 
                std::vector<Move>::iterator lastMoveIterator = getLastMoveIterator(targetColour);
                if (upLeftEmpty && (lastMoveIterator != moveVector.end()) && (lastMoveIterator->toIndex == target) && existsEnemyPiece(index, target)) { 
                    out.emplace_back(target);
                }

                target = shiftOne(index, getRight(boardVector[index].pieceColour));
                targetColour = boardVector[target].pieceColour;
                lastMoveIterator = getLastMoveIterator(targetColour);
                if (upRightEmpty && (lastMoveIterator != moveVector.end()) && (lastMoveIterator->toIndex == target) && existsEnemyPiece(index, target)) {
                    out.emplace_back(target);
                }
                return out;
            }

            std::vector<boardIndex> knightShift(boardIndex index) {
                std::vector<boardIndex> out {}; 
                boardIndex northSouth = shiftOne(index, Direction::NORTH);
                boardIndex eastWest = shiftOne(index, Direction::EAST);
                boardIndex diag = shiftOne(northSouth, Direction::NORTHEAST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = shiftOne(eastWest, Direction::NORTHEAST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = shiftOne(northSouth, Direction::NORTHWEST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = shiftOne(eastWest, Direction::NORTHWEST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                eastWest = shiftOne(index, Direction::WEST);
                northSouth = shiftOne(index, Direction::SOUTH);
                diag = shiftOne(northSouth, Direction::SOUTHEAST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = shiftOne(eastWest, Direction::SOUTHEAST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = shiftOne(northSouth, Direction::SOUTHWEST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = shiftOne(eastWest, Direction::SOUTHWEST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                return out;
            }

            // returns if the piece at i is a blocker
            bool isBlocker(boardIndex i) {
                return boardVector[i].type != PieceType::EMPTY;
            }
            // returns the list of on board ray indices in a given direction from a given src
            std::vector<boardIndex> getRay(boardIndex src, Direction d) {
                std::vector<boardIndex> out {};
                boardIndex next = shiftOne(src, d);
                while (boardVector[next].type == PieceType::EMPTY) {
                    out.emplace_back(next);
                    next = shiftOne(next, d);
                }
                // found blocker
                // add if capturable (blocker can be an out of bounds block piece)
                if (existsCapturable(src, next)) {
                    out.emplace_back(next);
                }
                return out;
            }
            std::vector<boardIndex> bishopShift(boardIndex src) {
                std::vector<boardIndex> out = getRay(src, Direction::NORTHEAST);
                concat(out, getRay(src, Direction::NORTHWEST));
                concat(out, getRay(src, Direction::SOUTHEAST));
                concat(out, getRay(src, Direction::SOUTHWEST));
                return out;
            }

            std::vector<boardIndex> rookShift(boardIndex src) {
                std::vector<boardIndex> out = getRay(src, Direction::NORTH);
                concat(out, getRay(src, Direction::EAST));
                concat(out, getRay(src, Direction::SOUTH));
                concat(out, getRay(src, Direction::WEST));
                return out;
            }

            std::vector<boardIndex> queenShift(boardIndex src) {
                return concat(rookShift(src), bishopShift(src));
            } 

            std::vector<boardIndex> kingShift(boardIndex src) {
                std::vector<Direction> directions = {Direction::NORTH, Direction::NORTHEAST, Direction::EAST, Direction::SOUTHEAST, Direction::SOUTH, Direction::SOUTHWEST, Direction::WEST, Direction::NORTHWEST};
                std::vector<boardIndex> out;
                boardIndex tmp;
                for (Direction d : directions) {
                    tmp = shiftOne(src, d);
                    if (existsCapturable(src, tmp)) {
                        out.emplace_back(tmp);
                    }
                }

                // calculate castling possibility
                if (boardVector[src].hasMoved) {
                    return out;
                }
                auto leftRay = getRay(src, getLeft(boardVector[src].pieceColour));
                auto rightRay = getRay(src, getRight(boardVector[src].pieceColour)); 

                if (leftRay.size() > 0) {
                    auto endLeftIndex = leftRay.back();
                    if (boardVector[endLeftIndex].type == PieceType::ROOK && boardVector[endLeftIndex].hasMoved == false) {
                        out.emplace_back(endLeftIndex);
                    }
                }
                if (rightRay.size() > 0) {
                    auto endRightIndex = rightRay.back();
                    if (boardVector[endRightIndex].type == PieceType::ROOK && boardVector[endRightIndex].hasMoved == false) {
                        out.emplace_back(endRightIndex);
                    }
                }
                return out;
            }

            bool isCastlingMove(Move m) {
                if (!(m.fromSquare.type == PieceType::KING && m.toSquare.type == PieceType::ROOK)) {
                    return false;
                }
                boardIndex src = m.fromIndex;
                boardIndex tgt = m.toIndex;
                short diff = tgt - src;

                std::vector<short> oneShiftValues = {1, 15, 16, 17, -1, -15, -16, -17};
                for (short v : oneShiftValues) {
                    if (v == diff) {
                        return false;
                    }
                }
                return true;
            }

            // assumes there is a valid non diagonal straight path towards tgt from src
            Direction getDirection(boardIndex src, boardIndex tgt) {
                short diff = tgt - src;
                if (diff > 0) {
                    if (diff > PADDEDCOLS) {
                        return Direction::NORTH;
                    }
                    return Direction::EAST;
                }
                if (-diff > PADDEDCOLS) {
                    return Direction::SOUTH;
                }
                return Direction::WEST;
            }

            bool isEnPeasant(const Move m) {
                if (m.toIndex != shiftOne(m.fromIndex, getLeft(m.fromSquare.pieceColour)) || m.toIndex != shiftOne(m.fromIndex, getRight(m.fromSquare.pieceColour))) {
                    return false;
                }
                if (m.fromSquare.type != PieceType::PAWN || m.toSquare.type != PieceType::PAWN) {
                    return false;
                }
                return true;
            }

            bool willPromote(const Move m) {
                switch (m.fromSquare.pieceColour) {
                    case PieceColour::RED:
                        return m.toIndex > 128;
                    case PieceColour::BLUE:
                        return m.toIndex % 16 >= 8;
                    case PieceColour::YELLOW:
                        return m.toIndex < 143;
                    case PieceColour::GREEN:
                        return m.toIndex <= 7;
                    default:
                        return false;
                }
                return false;
            }
            
            // checks if indices constitute a capture with relation to the board
            // bool isCapture(boardIndex src, boardIndex trgt) {
            //     return existsEnemyPiece(src, trgt);
            // }

            void playMove(const Move m) {
                // runtime check that everything is working smoothly
                if ((boardVector[m.fromIndex].type != m.fromSquare.type) || (boardVector[m.toIndex].type != m.toSquare.type)) {
                    throw std::invalid_argument("Move is poorly formed: board square does not match move square data ");
                }

                if (boardVector[m.fromIndex].pieceColour != turn) {
                    std::string s = "It is not the given move's turn. Turn: " + std::string(PieceColourToString(turn)) + " pieceColour: " + std::string(PieceColourToString(m.fromSquare.pieceColour)) + " board piece colour: " + std::string(PieceColourToString(boardVector[m.fromIndex].pieceColour)) + "\n";
                    throw std::invalid_argument(s);
                }

                turn = getNextTurn(turn);

                // bool isCapture = false;
                // if (existsEnemyPiece(m.fromIndex, m.toIndex)) {
                //     isCapture = true;
                // }



                // promotion
                if (willPromote(m)) {
                    
                    // assume it will promote to queen
                    boardVector[m.toIndex].setPiece(m.fromSquare);
                    boardVector[m.toIndex].type = PieceType::QUEEN; // promote the type
                    boardVector[m.fromIndex].setPiece(EMPTYPIECE);
                    // if (isCapture) {
                    //     moveVector.emplace_back(Move(m.fromSquare, m.fromIndex, m.toSquare, m.toIndex, m.toSquare, PieceType::QUEEN));
                    //     return;
                    // }
                    moveVector.emplace_back(Move(m.fromSquare, m.fromIndex, m.toSquare, m.toIndex, PieceType::QUEEN));
                    return;
                }

                if (isEnPeasant(m)) {
                    boardIndex tmp = shiftOne(m.toIndex, getUp(m.toSquare.pieceColour));
                    boardVector[tmp].setPiece(m.fromSquare);
                    boardVector[tmp].hasMoved = true;
                    boardVector[m.fromIndex].setPiece(EMPTYPIECE);
                    boardVector[m.toIndex].setPiece(EMPTYPIECE);
                    moveVector.emplace_back(m);
                    return;
                }


                if (isCastlingMove(m)) {
                    Direction dir = getDirection(m.fromIndex, m.toIndex);
                    boardIndex pos = shiftOne(m.fromIndex, dir);
                    // copy tgt to 1 move in direction
                    boardVector[pos].setPiece(m.toSquare);
                    boardVector[pos].hasMoved = true;
                    pos = shiftOne(pos, dir);
                    // copy src to 2 moves in direction
                    boardVector[pos].setPiece(m.fromSquare);
                    boardVector[m.fromIndex].setPiece(EMPTYPIECE);
                    boardVector[m.toIndex].setPiece(EMPTYPIECE);
                    moveVector.emplace_back(m);
                    return;
                }
                boardVector[m.toIndex].pieceColour = boardVector[m.fromIndex].pieceColour;
                boardVector[m.toIndex].type = boardVector[m.fromIndex].type;
                boardVector[m.toIndex].hasMoved = true;
                boardVector[m.fromIndex].type = PieceType::EMPTY;
                boardVector[m.fromIndex].pieceColour = PieceColour::NONE;
                moveVector.emplace_back(m);
            }

            // unplays the last played move
            void unPlayMove() {
                Move lastMove = moveVector.back(); //retreive last elem 
                boardVector[lastMove.toIndex].setPiece(lastMove.toSquare); //return data to state
                boardVector[lastMove.fromIndex].setPiece(lastMove.fromSquare); 
                turn = getPrevTurn(turn);// reset turn counter
                moveVector.pop_back(); //remove from stack

            }
    };
};
// int main() {
//     // main for debugging / testing types
//     Board b = Board();
//     b.printBoard();
//     std::vector<Move> moves = b.generateLegalMoves();
//     b.playMove(moves.at(0));
//     std::cout << "board after first move is played\n";
//     b.printBoard();
//     std::cout << "moves length: " << moves.size() << "\n";
//     // std::cout << "first move: " << moveToString(moves[0]) << "\n";
//     printMoveVector(moves);
// }

#endif;