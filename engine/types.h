#include<memory>
#include<iostream>
#include<cassert>
#include<exception>

using namespace std;

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

    struct Piece {
        PieceColour pieceColour;
        PieceType type;
        bool hasMoved;
        boardIndex positionIndex;
        Piece(
            PieceColour c = PieceColour::NONE, 
            PieceType t = PieceType::EMPTY, 
            bool m = false, 
            boardIndex i = 300
        ) :
        pieceColour(c), 
        type(t), 
        hasMoved(m), 
        positionIndex(i) {
        }
    };

    enum class SquareColour {
        DARK,
        LIGHT,
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
            Square(bool access, PieceType type, PieceColour col) :
            canAccess(access), pieceType(type), pieceColour(col) {
                if ((!access && (type != PieceType::BLOCK || col != PieceColour::NONE)) || 
                (access && type == PieceType::BLOCK)) {
                    throw invalid_argument("incorrect configuration for calling square constructor");
                }
            }

            bool isAccessible() const {
                return canAccess;
            }

            bool isEmpty() const {
                return pieceType == PieceType::EMPTY && pieceColour == PieceColour::NONE;
            }
            
            void setPiece(PieceType t, PieceColour c) {
                assert(canAccess);
                pieceType = t;
                pieceColour = c;
            }

            PieceType type() {
                return pieceType;
            }

            PieceColour colour() const {
                return pieceColour;
            }

    };

    // move is a container for squares and indices
    class Move {
        private:
            // board data represents the involved pieces
            // board
            boardIndex fromI;
            boardIndex toI;
            PieceColour fromC;
            PieceType fromP;
            // capture
            PieceColour capturedC;
            PieceType capturedP;
            // special move defined as a castling or en peasant move
            bool special;
            // promotion
            PieceType toP;

        public:
            int totalMoves; // holds how many other moves couldve been chosen at the time amongst this move
            
            Move() : 
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
            Move(const Move &m) = default;
            Move(Move &&m) = default;
            Move& operator=(const Move&) = default;

            // generic move
            Move(
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


            const bool isPromotion() const {
                return fromP != PieceType::EMPTY && toP != PieceType::EMPTY;
            }
            const bool isCapture() const {
                return capturedP != PieceType::EMPTY && capturedP != PieceType::BLOCK;
            }
            const bool isSpecial() const {
                return special;
            }
            const bool isCastling() const {
                return special && fromC == capturedC && fromP == PieceType::KING && capturedP == PieceType::ROOK;
            }
            const bool isEnPeasant() const {
                return special && fromC != capturedC && capturedC == PieceColour::NONE && capturedP == PieceType::EMPTY && fromP == PieceType::PAWN;
            }
            PieceType capturedPiece() const {
                return capturedP;
            }
            PieceColour capturedColour() const {
                return capturedC;
            }
            const boardIndex fromIndex() const {
                return fromI;
            }
            constexpr boardIndex toIndex() const {
                return toI;
            }
            const PieceType fromPiece() const {
                return fromP;
            }
            const PieceType promotionPiece() const {
                return  toP;
            }
            const PieceColour fromColour() const {
                return fromC;
            }
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


};
#endif