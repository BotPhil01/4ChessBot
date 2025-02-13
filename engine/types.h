#include<cassert>
#include<stdint.h>

#ifndef TYPES_H
#define TYPES_H

namespace types {
    using boardIndex = uint_fast16_t;

    // piece enums
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
    
    enum class PieceColour {
        RED,
        BLUE,
        YELLOW,
        GREEN,
        NONE, //NON PIECE SQUARES
    };

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

    
    class Square {
        private:
            bool canAccess;
            PieceType pieceType;
            PieceColour pieceColour;
        public:
            constexpr Square() :
            canAccess(false), pieceType(PieceType::EMPTY), pieceColour(PieceColour::NONE) {                
            }
            constexpr Square(const bool access, const PieceType type, const PieceColour col) :
            canAccess(access), pieceType(type), pieceColour(col) {
                assert (
                    (!access && (type == PieceType::BLOCK && col == PieceColour::NONE)) || 
                    (access && type != PieceType::BLOCK)
                );
            }

            constexpr bool isAccessible() const {
                return canAccess;
            }

            constexpr bool isEmpty() const {
                return pieceType == PieceType::EMPTY && pieceColour == PieceColour::NONE;
            }
            
            constexpr void setPiece(PieceType t, PieceColour c) {
                assert(canAccess);
                pieceType = t;
                pieceColour = c;
            }

            constexpr PieceType type() const {
                return pieceType;
            }

            constexpr PieceColour colour() const {
                return pieceColour;
            }

    };

    // move is a container for squares and indices
    class Move {
        private:
            // board data represents the involved pieces
            // board
            boardIndex fromI; // 2byte
            boardIndex toI; // 2byte
            PieceColour fromC; // 4 byte
            PieceType fromP; // 4 byte
            // capture
            PieceColour capturedC; // 4 byte
            PieceType capturedP; // 4 byte
            // special move defined as a castling or en peasant move
            bool special; // 1 byte
            // promotion
            PieceType toP; // 4 byte

        public:
            // 2 byte 
            int totalMoves; // holds how many other moves couldve been chosen at the time amongst this move
            unsigned short index = 255; // index in moves 

            constexpr Move() : 
            fromI(300), 
            toI(300), 
            fromC(PieceColour::NONE), 
            fromP(PieceType::EMPTY),
            capturedC(PieceColour::NONE),
            capturedP(PieceType::EMPTY), 
            special(false), 
            toP(PieceType::EMPTY),
            totalMoves(0)  {
            }

            // Move() = default;
            constexpr Move(const Move &m) = default;
            constexpr Move(Move &&m) = default;
            constexpr Move& operator=(const Move&) = default;

            bool operator==(const Move&) const = default;

            // generic move
            constexpr Move(
            const boardIndex fi, 
            const boardIndex ti, 
            const int total, 
            const PieceType fp, 
            const PieceColour fromCol,
            const PieceType captured, 
            const PieceColour capturedCol, 
            const PieceType tp = PieceType::EMPTY, 
            bool isSpecial = false) : 

            fromI(fi), 
            toI(ti), 
            fromC(fromCol),
            fromP(fp), 
            capturedC(capturedCol),
            capturedP(captured), 
            special(isSpecial), 
            toP(tp), 
            totalMoves(total) 
            {
            }


            constexpr bool isPromotion() const {
                return fromP != PieceType::EMPTY && toP != PieceType::EMPTY;
            }
            constexpr bool isCapture() const {
                return capturedP != PieceType::EMPTY && capturedP != PieceType::BLOCK;
            }
            constexpr bool isSpecial() const {
                return special;
            }
            constexpr bool isCastling() const {
                return special && fromC == capturedC && fromP == PieceType::KING && capturedP == PieceType::ROOK;
            }
            constexpr PieceType capturedPiece() const {
                return capturedP;
            }
            constexpr PieceColour capturedColour() const {
                return capturedC;
            }
            constexpr boardIndex fromIndex() const {
                return fromI;
            }
            constexpr boardIndex toIndex() const {
                return toI;
            }
            constexpr PieceType fromPiece() const {
                return fromP;
            }
            constexpr PieceType promotionPiece() const {
                return  toP;
            }
            constexpr PieceColour fromColour() const {
                return fromC;
            }
    };

    // fixed length move array 
    // maximum length of moves created in 1 call of pseudoLegalMoves = K: 10 * 1 Q: 41 * 9 R: 26 * 2: B: 15 * 2 P: 4 * 0 = 461  
    // class FLMarray {
    //     constexpr static std::size_t maxSize = 461;
    //     std::array<Move, maxSize + 1> arr = {};
    //     unsigned int filled = 0;
    //     public:
    //         constexpr FLMarray() = default;
    //         constexpr ~FLMarray() = default;
    //         constexpr Move at(unsigned int i) const {
    //             assert(i < filled);
    //             return arr[i];
    //         }
    //         void emplace_back(Move move) {
    //             assert(filled < arr.size());
    //             arr[filled++] = move;
    //         }
    //         constexpr std::size_t size() const {
    //             return filled;
    //         }

    //         constexpr std::size_t max_size() const {
    //             return maxSize;
    //         }
    // };


    // fixed length index array for storing indices after a single shift
    // maximum theorhetical for shifts is a queen shift 13 + 13 + 4 + 4 + 4 + 3 = 41
    // class FLSArray {
    //     // initialises to 0
    //     const static std::size_t maxSize = 41;
    //     const static boardIndex END = 400;
    //     std::array<boardIndex, maxSize + 1> arr = {};
    //     volatile std::size_t filled = 0UL;
    //     public:
    //         constexpr FLSArray() {
    //             arr[maxSize + 1] = END;
    //         }
    //         constexpr ~FLSArray() = default;
    //         // constexpr boardIndex at(unsigned int i) const {
    //         //     assert(i < filled);
    //         //     return arr[i];
    //         // }
    //         constexpr boardIndex operator[](unsigned int i) const {
    //             assert(i <= maxSize);
    //             return arr[i];
    //         }
    //         void emplace_back(boardIndex index) {
    //             assert(filled <= maxSize);
    //             arr[filled++] = index;
    //         }
    //         constexpr std::size_t size() const {
    //             return filled;
    //         }
    //         constexpr std::size_t max_size() const {
    //             return maxSize;
    //         }
    //         constexpr types::boardIndex back() const {
    //             assert(filled != 0);
    //             return arr[filled-1];
    //         }
    //         constexpr types::boardIndex end() const {
    //             return arr[maxSize + 1];
    //         }

    // };

};
#endif