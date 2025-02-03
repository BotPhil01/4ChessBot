#include<cassert>
#include<stdint.h>

#ifndef TYPES_H
#define TYPES_H

namespace types {
    using boardIndex = uint16_t;

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
            Square() :
            canAccess(false), pieceType(PieceType::EMPTY), pieceColour(PieceColour::NONE) {                
            }
            Square(const bool access, const PieceType type, const PieceColour col) :
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
            
            void setPiece(PieceType t, PieceColour c) {
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



};
#endif