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
        bool hasMoved ;
        Piece(PieceColour c = PieceColour::NONE, PieceType t = PieceType::EMPTY, bool m = false) :
        pieceColour(c), type(t), hasMoved(m) {
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

    // inherits move
    struct Promotion {
        PieceType newType;
        Promotion(PieceType o = PieceType::QUEEN) 
        : newType(o) {
        }
    };
    
    class Capture {
        private: 
            bool captureHappened; // asserts whether any piece has been captured yet
            Piece capturedPiece = Piece(); //default to an empty capture
            bool checkConstraints(Piece p) {
                return p.type != PieceType::EMPTY && p.type != PieceType::BLOCK && p.pieceColour != PieceColour::NONE;
            }
        public:
            Capture() {
            }
            Capture(Piece p) {
                setCapture(p);
            }
            bool checkCaptureHappened() {
                return captureHappened;
            }
            void setCapture(Piece p) {
                if (checkConstraints(p)) {
                    captureHappened = true;
                    capturedPiece = p;
                }
            }

    };

    // move is a container for squares and indices
    struct Move : Promotion, Capture {
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
        Move(Square fs, boardIndex fi, Square ts, boardIndex ti) :
        fromSquare(fs), fromIndex(fi), toSquare(ts), toIndex(ti) {
        }
        Move(Square fs, boardIndex fi, Square ts, boardIndex ti, PieceType p) :
        Promotion(p), 
        fromSquare(fs), fromIndex(fi), toSquare(ts), toIndex(ti) {
        }
        Move(Square fs, boardIndex fi, Square ts, boardIndex ti, Piece c) :
        Capture(c),
        fromSquare(fs), fromIndex(fi), toSquare(ts), toIndex(ti){
        }
        Move(Square fs, boardIndex fi, Square ts, boardIndex ti, Piece c, PieceType p) : 
        Promotion(p), Capture(c),  
        fromSquare(fs), fromIndex(fi), toSquare(ts), toIndex(ti) {
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