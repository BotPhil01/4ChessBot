#include<memory>
#include<iostream>

#ifndef TYPES_H
#define TYPES_H
namespace types {
    using boardIndex = std::uint16_t;

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
        Piece(PieceColour c = PieceColour::NONE, PieceType t = PieceType::EMPTY, bool m = false, boardIndex i = 300) :
        pieceColour(c), type(t), hasMoved(m), positionIndex(i) {
        }
    };

    enum class SquareColour {
        DARK,
        LIGHT,
    };

    class Square : public Piece{
        private:
            SquareColour squareColour;
            
        public:
            Square(SquareColour sc = SquareColour::DARK, PieceColour pc = PieceColour::NONE, PieceType pt = PieceType::BLOCK, bool moved = false) :
            Piece(pc, pt, moved), squareColour(sc) {
            }
            bool isCapturble(Piece _p) {
                return type == PieceType::EMPTY || pieceColour != _p.pieceColour;
            }
            // sets type and colour to p type and colour
            void setPiece(Piece p) {
                type = p.type;
                pieceColour = p.pieceColour;
            }
            SquareColour getSquareColour() {
                return squareColour;
            }
            
    };

    // move is a container for squares and indices
    class Move {
        private:
            boardIndex fromI;
            boardIndex toI;
            PieceColour fromC;
            // capture
            PieceType capturedP;
            PieceColour capturedC;
            // promotion
            PieceType fromP;
            PieceType toP;

        public:
            int totalMoves; // holds how many other moves couldve been chosen at the time amongst this move
            
            Move() : 

            fromI(300), toI(300), 
            totalMoves(0), fromC(PieceColour::NONE),
            capturedP(PieceType::EMPTY), capturedC(PieceColour::NONE),
            fromP(PieceType::EMPTY), toP(PieceType::EMPTY)  {
            }

            Move(const boardIndex fi, const boardIndex ti, 
            const int total = 0, const PieceColour fromCol, 
            const PieceType captured, const PieceColour capturedCol) :

            fromI(fi), toI(ti), totalMoves(total), fromC(fromCol),
            capturedP(captured), capturedC(capturedCol), 
            fromP(PieceType::EMPTY), toP(PieceType::EMPTY) {
            }

            Move(const boardIndex fi, const boardIndex ti, 
            const int total, const PieceColour fromCol,
            const PieceType captured, const PieceColour capturedCol, 
            const PieceType fp, const PieceType tp = PieceType::EMPTY) : 

            fromI(fi), toI(ti), totalMoves(total), fromC(fromCol),
            capturedP(captured), capturedC(capturedCol),
            fromP(fp), toP(tp) {
            }

            Move(Move &m) {
                fromI = m.fromIndex();
                toI = m.toIndex();
                capturedP = m.capturedPiece();
                fromP = m.fromPiece();
                toP = m.toPiece();
                capturedC = m.capturedColour();
            }

            const bool isPromotion() {
                return fromP != PieceType::EMPTY && toP != PieceType::EMPTY;
            }
            const bool isCapture() {
                return capturedP != PieceType::EMPTY && capturedP != PieceType::BLOCK;
            }
            PieceType capturedPiece() {
                return capturedP;
            }
            PieceColour capturedColour() {
                return capturedC;
            }
            const boardIndex fromIndex() {
                return fromI;
            }
            const boardIndex toIndex() {
                return toI;
            }
            const PieceType fromPiece() {
                return fromP;
            }
            const PieceType toPiece() {
                return  toP;
            }
            const PieceColour fromColour() {
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