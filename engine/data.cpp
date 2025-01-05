#include<vector>
#include<memory>
#include<iostream>
#include<string>
#include<sstream>
#include<algorithm>
#include<exception>
using boardIndex = std::uint16_t;
// module for defining the board encoding / representation

// options:
// square centric
// piece centric
// go with bitboards ?
// if board is 14 x 14 this requires a 196bit word which is unrealistic hence need a way to encapsulate a bitboard in a 4d chess
// max is 64 bits hence requires at least 3 bit boards to cover all squares more realistically 4 though 
// main advantage of bitboards is to use 1 variable per piece hence need another solution
// 
// encode board as an array = 14 x 14 board
// 



enum class PieceColour {
    RED,
    BLUE,
    YELLOW,
    GREEN,
    NONE,
};

constexpr std::string_view PieceCOlourToString(PieceColour t) {
    using pc = PieceColour;
    switch (t) {
        case pc::RED:
            return "RED";
        case pc::GREEN:
            return "GREEN";
        case pc::BLUE:
            return "BLUE";
        case pc::YELLOW:
            return "YELLOW";
        case pc::NONE:
            return "NONE";
        default:
            return "NOCOLOUR";

    };
}

enum class PieceType { // not set to class to allow for indeexing into pieceTypes[]
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
    EMPTY, //vacant squares
    BLOCK, //non existent squares
};

constexpr std::string_view PieceTypeToString(PieceType t) {
    using pt = PieceType;
    switch (t) {
        case pt::PAWN:
            return "PAWN";
        case pt::ROOK:
            return "ROOK";
        case pt::KNIGHT:
            return "KNIGHT";
        case pt::BISHOP:
            return "BISHOP";
        case pt::QUEEN:
            return "QUEEEN";
        case pt::KING:
            return "KING";
        case pt::EMPTY:
            return "EMPTY";
        default:
            return "BLOCK";
    };
}

struct Piece {
    PieceColour pieceColour;
    PieceType type;
};

enum class SquareColour {
    DARK,
    LIGHT,
};

class Square : public Piece{
    private:
        SquareColour squareColour;
    public:
        Square(SquareColour sc = SquareColour::DARK, PieceColour pc = PieceColour::NONE, PieceType pt = PieceType::BLOCK) {
            pieceColour = pc;
            type = pt;
            squareColour = sc;
        }
        bool isCapturable() {
            return type == PieceType::EMPTY;
        }
        bool isCapturble(Piece _p) {
            return type == PieceType::EMPTY || pieceColour != _p.pieceColour;
        }
        void setPiece(Piece p) {
            type = p.type;
            pieceColour = p.pieceColour;
        }
        // returns piece type as string view
        std::string_view toStringView() {
            return PieceTypeToString(type);
        }
        
};



namespace boardInit {
    const std::vector<boardIndex> cornerIndices = { 33, 34, 35, 49, 50, 51, 65, 66, 67,
                                                        44, 45, 46, 60, 61, 62, 76, 77, 78,
                                                        209, 210, 211, 225, 226, 227, 241, 242, 243,
                                                        220, 221, 222, 236, 237, 238, 252, 253, 254};
                                                        
    bool isRedStart(short i) {
        return i > 35 && i < 60;
    }

    bool isYellowStart(short i) {
        return i > 227 && i < 252;
    }

    bool isBlueStart(short i) {
        i = i % 16;
        return i == 1 || i == 2;
    }

    bool isGreenStart(short i) {
        i = i % 16;
        return i == 13 || i == 14;
    }

    bool isPawnStart(short i) {
        if ((i > 51 && i < 60) || (i > 227 && i < 236)) { // red and yellow
            return true;
        }
        short rem = i % 16;
        return (i > 81 && i < 207 && (rem == 13 || rem == 2));
    }

    bool isRookStart(short i) {
        return i == 36 || i == 43 || i == 81 || i == 94 || i == 193 || i == 206 || i == 244 || i == 251;
    }

    bool isKnightStart(short i) {
        return i == 37 || i == 42 || i == 97 || i == 110 || i == 177 || i == 190 || i == 245 || i == 250;
    }

    bool isBishopStart(short i) {
        return i == 38 || i == 41 || i == 113 || i == 126 || i == 161 || i == 174 || i == 246 || i == 249;
    }

    bool isQueenStart(short i) {
        return i == 39 || i == 129 || i == 158 || i == 248;
    }

    bool isKingStart(short i) {
        return i == 40 || i == 145 || i == 142 || i == 247;
    }

    bool isEmptyStart(short i) {
        short tmp = i % 16;
        bool in14Square = (i > 32 && i < 255 && tmp != 15 && tmp != 0);
        bool notInCorners = true;
        for (boardIndex j : boardInit::cornerIndices) {
            if (i == j) {
                notInCorners = false;
                break;
            }
        }
        return notInCorners && in14Square;
        
    }
    

    Square generateSquare(short i) {
        // using namespace boardInit;
        SquareColour sc = i % 2 == 0 ? SquareColour::DARK : SquareColour::LIGHT;
        // assign colour
        PieceColour pc = PieceColour :: NONE;
        pc = isRedStart(i) ? PieceColour::RED : isBlueStart(i) ? PieceColour::BLUE : isYellowStart(i) ? PieceColour::YELLOW : isGreenStart(i) ? PieceColour::GREEN : PieceColour::NONE;
        // assign type
        PieceType pt = PieceType::BLOCK;
        pt = isEmptyStart(i) ? PieceType::EMPTY : pt; // empty 
        pt = isRookStart(i) ? PieceType::ROOK : isKnightStart(i) ? PieceType::KNIGHT : isBishopStart(i) ? PieceType::BISHOP : pt;
        pt = isPawnStart(i) ? PieceType::PAWN : isKingStart(i) ? PieceType::KING : isQueenStart(i) ? PieceType::QUEEN : pt;
        return Square(sc, pc, pt);
    };

    const PieceColour initailTurn = PieceColour::RED;
}

enum class Direction {
    NORTH,
    NORTHEAST,
    EAST,
    SOUTHEAST,
    SOUTH,
    SOUTHWEST,
    WEST,
    NORTHWEST,
};


struct Move {
    Square fromSquare;
    boardIndex fromIndex;
    Square toSquare;
    boardIndex toIndex;
    Move() {
        fromSquare = Square();
        toSquare = Square();
        fromIndex = 300;
        toIndex = 300;
    }
    Move(Square fs, boardIndex bi, Square ts, boardIndex ti) {
        fromSquare = fs;
        fromIndex = bi;
        toSquare = ts;
        toIndex = ti;
    }
    std::string toString() {
        std::string s = std::string(fromSquare.toStringView());
        s.append(std::to_string(fromIndex));
        s.append(toSquare.toStringView());
        s.append(std::to_string(toIndex));
        return s;
    }
};


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
            using namespace boardInit;
            // construct to initial position
            for (short i = 0; i < 288; ++i) {
                boardVector.emplace_back(generateSquare(i));
            }
            turn = initailTurn;
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

        // helper function to query board if the piece at s can capture at t also checking if t exists (can be anything on board)
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
            // BUG last element emplaced turns to block and trgt turns to 30
            return out;
        }

        Move createMove(boardIndex src, boardIndex trgt) {
            Square srcSquare = boardVector[src];
            Square trgtSquare = boardVector[trgt];
            return Move(srcSquare, src, trgtSquare, trgt);
        }

        std::vector<Move> generatePseudoLegalMoves() {
            // done by index shifting
            // basic idea:
            // 1) get all pieces belonging to Board.turn
            // 2) for each piece we want to calculate its "possible" moves -> possible defined as the squares it can move to
            // 2.a) according to the type shift the index according eg: pawn shift = +16
            // TODO Optimise later with piece lists 
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
                        // case PieceType::ROOK:
                        //     out = concat(out, bulkCreateMove(i, rookShift(i)));
                        //     break;
                        // case PieceType::KNIGHT:
                        //     out = concat(out, bulkCreateMove(i, knightShift(i)));
                        //     break;
                        // case PieceType::BISHOP:
                        //     out = concat(out, bulkCreateMove(i, bishopShift(i)));
                        //     break;
                        // case PieceType::KING:
                        //     out = concat(out, bulkCreateMove(i, kingShift(i)));
                        //     break;
                        // case PieceType::QUEEN:
                        //     out = concat(out, bulkCreateMove(i, queenShift(i)));
                        //     break;
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
            
            std::cout << "out[0]: " << out[0].toString() << "\n";
            std::cout << "out[0] with type: " << PieceTypeToString(out[0].fromSquare.type) << "\n";
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
            for (boardIndex c : boardInit::cornerIndices) { // not in corners
                if (c == i) {
                    return false;
                }
            }
            boardIndex rem = i % 16;
            return rem != 0 && rem != 15 && i < 252 && i > 35;
        }

        template <typename T>
        std::vector<T> concat(std::vector<T> v1, std::vector<T> v2) {
            for (T i : v2) {
                v1.emplace_back(i);
            }
            return v1;
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
            if (!boardInit::isPawnStart(index)) {
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
            boardIndex target = shiftOne(index, Direction::NORTHEAST);
            if (existsCapturable(index, target)) {
                out.emplace_back(target);
            }

            target = shiftOne(index, Direction::NORTHWEST); // go to next possible capture
            if (existsCapturable(index, target)) {
                out.emplace_back(target);
            }

            // en peasant
            target = shiftOne(index, Direction::EAST);
            // std::cout << (*moveVector[target]).to << std::endl;

            PieceColour targetColour = boardVector[target].pieceColour; // error invalid read size of 8
            std::vector<Move>::iterator lastMoveIterator = getLastMoveIterator(targetColour);
            if ((lastMoveIterator != moveVector.end()) && (lastMoveIterator->toIndex == target) && existsEnemyPiece(index, target)) { 
                out.emplace_back(target);
            }

            target = shiftOne(index, Direction::WEST);
            targetColour = boardVector[target].pieceColour;
            lastMoveIterator = getLastMoveIterator(targetColour);
            if ((lastMoveIterator != moveVector.end()) && (lastMoveIterator->toIndex == target) && existsEnemyPiece(index, target)) {
                out.emplace_back(target);
            }
            return out;
        }

        std::vector<boardIndex> knightShift(boardIndex index) {
            // TODO FIX THIS FOR ALL KNIGHT MOVES NOT JUST FRONT AND BACK
            std::vector<boardIndex> out {}; 
            boardIndex northSouth = shiftOne(index, Direction::NORTH);
            boardIndex diag = shiftOne(northSouth, Direction::NORTHEAST);
            if (existsCapturable(index, diag)) {
                std::cout << diag << " is capturable from " << index << "\n";
                out.emplace_back(diag);
            }
            diag = shiftOne(northSouth, Direction::NORTHWEST);
            if (existsCapturable(index, diag)) {
                std::cout << diag << " is capturable from " << index << "\n";
                std::cout << "fromSquare colour: " << PieceCOlourToString(boardVector[index].pieceColour) << "\n";
                out.emplace_back(diag);
            }
            northSouth = shiftOne(index, Direction::SOUTH);
            diag = shiftOne(northSouth, Direction::SOUTHEAST);
            if (existsCapturable(index, diag)) {
                std::cout << diag << " is capturable from " << index << "\n";
                out.emplace_back(diag);
            }
            diag = shiftOne(northSouth, Direction::SOUTHWEST);
            if (existsCapturable(index, diag)) {
                std::cout << diag << " is capturable from " << index << "\n";
                out.emplace_back(diag);
            }
            std::cout << out.size() << " size \n";
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
            return out;
        }

        void playMove(const Move m) {
            // runtime check that everything is working smoothly
            if ((boardVector[m.fromIndex].type != m.fromSquare.type) || (boardVector[m.toIndex].type != m.toSquare.type)) {
                throw std::invalid_argument("Move is poorly formed: board square does not match move square data ");
            }
            boardVector[m.toIndex].pieceColour = boardVector[m.fromIndex].pieceColour;
            boardVector[m.toIndex].type = boardVector[m.fromIndex].type;
            boardVector[m.fromIndex].type = PieceType::EMPTY;
            boardVector[m.fromIndex].pieceColour = PieceColour::NONE;
            moveVector.emplace_back(m);
        }

};

int main() {
    // main for debugging / testing types
    Board b = Board();
    b.printBoard();
    std::vector<Move> moves = b.generatePseudoLegalMoves();
    std::cout << "moves length: " << moves.size() << "\n";
    std::cout << "first move: " << moves[0].toString() << "\n";
    b.playMove(moves.at(0));
    std::cout << "board after first move is played\n";
    b.printBoard();
}
