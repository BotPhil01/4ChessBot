#ifndef MOVE_H
#define MOVE_H
#include "bitboard.h"
#include "boardDefaults.h"
#include "types.h"
namespace move {
    // move is a container for squares and indices
    class Move {
        public:
            // the sum board of the src
            bitboard::Bitboard &sumBoard;
            // the board where the shift comes from
            bitboard::Bitboard &srcBoard;
            // the isolated bit from srcboard
            bitboard::Bitboard unarySrcBoard;
            // the shift that it represents
            bitboard::Bitboard destBoard;
            // the special board
            bitboard::Bitboard specialBoard; // 1 byte
            types::PieceColour d_fromColour;
            types::PieceColour d_toColour;
            types::PieceType d_fromPiece;
            types::PieceType d_toPiece;
            // promotion
            types::PieceType toP; // 4 byte
            // 2 byte 
            int totalMoves; // holds how many other moves couldve been chosen at the time amongst this move
            unsigned short index = 255; // index in moves 


            constexpr Move(const Move &m) : 
                d_fromColour(m.d_fromColour),
                d_toColour(m.d_toColour),
                d_fromPiece(m.d_fromPiece),
                d_toPiece(m.d_toPiece),
                sumBoard(m.sumBoard),
            srcBoard(m.srcBoard),
            unarySrcBoard(m.unarySrcBoard),
            destBoard(m.destBoard),
            specialBoard(m.specialBoard),
            toP(m.toP),
            totalMoves(m.totalMoves),
            index(m.index)
            {}

            constexpr Move(Move &&m) : 
                d_fromColour(m.d_fromColour),
                d_toColour(m.d_toColour),
                d_fromPiece(m.d_fromPiece),
                d_toPiece(m.d_toPiece),
            sumBoard(m.sumBoard),
            srcBoard(m.srcBoard),
            unarySrcBoard(m.unarySrcBoard),
            destBoard(m.destBoard),
            specialBoard(m.specialBoard),
            toP(m.toP),
            totalMoves(m.totalMoves),
            index(m.index)
            {}
            
            constexpr Move& operator=(const Move& m) {
                d_fromColour = m.d_fromColour;
                d_toColour = m.d_toColour;
                d_fromPiece = m.d_fromPiece;
                d_toPiece = m.d_toPiece;
                sumBoard = m.sumBoard;
                srcBoard = m.srcBoard;
                unarySrcBoard = m.unarySrcBoard;
                destBoard = m.destBoard;
                specialBoard = m.specialBoard;
                toP = m.toP;
                totalMoves = m.totalMoves;
                index = m.index;
                return *this;
            }

            // generic move
            Move(
                    bitboard::Bitboard &sumBoard,
                    bitboard::Bitboard &srcBoard,
                    const bitboard::Bitboard unarySrcBoard,
                    const bitboard::Bitboard destBoard,
                    const bitboard::Bitboard specialBoard = boardDefaults::zeroed,
                    types::PieceColour d_fromColour = types::PieceColour::NONE,
                    types::PieceColour d_toColour = types::PieceColour::NONE,
                    types::PieceType d_fromPiece = types::PieceType::EMPTY,
                    types::PieceType d_toPiece = types::PieceType::EMPTY,
                    const types::PieceType tp = types::PieceType::EMPTY, 
                    const int total = 0, 
                    const int index = 0
            ) : 
                d_fromColour(d_fromColour),
                d_toColour(d_toColour),
                d_fromPiece(d_fromPiece),
                d_toPiece(d_toPiece),
                sumBoard(sumBoard),
                srcBoard(srcBoard),
                unarySrcBoard(unarySrcBoard),
                destBoard(destBoard),
                specialBoard(specialBoard),
                toP(tp), 
                totalMoves(total),
                index(index)
            {}

             Move() :
                d_fromColour(types::PieceColour::NONE),
                d_toColour(types::PieceColour::NONE),
                d_fromPiece(types::PieceType::EMPTY),
                d_toPiece(types::PieceType::EMPTY),
                sumBoard(bitboard::nullBoard),
                srcBoard(bitboard::nullBoard),
                unarySrcBoard(boardDefaults::nullBoard),
                destBoard(boardDefaults::nullBoard),
                specialBoard(boardDefaults::nullBoard),
                toP(types::PieceType::EMPTY),
                totalMoves(0),
                index(0)
           {}

            bool operator==(const Move &other) const {
                    return (
                            d_fromColour == other.d_fromColour &&
                            d_toColour == other.d_toColour &&
                            d_fromPiece == other.d_fromPiece &&
                            d_toPiece == other.d_toPiece &&
                            // sumBoard == other.sumBoard &&
                            // srcBoard == other.srcBoard &&
                            unarySrcBoard == other.unarySrcBoard &&
                            destBoard == other.destBoard &&
                            specialBoard == other.specialBoard &&
                            toP == other.toP &&
                            totalMoves == other.totalMoves &&
                            index == other.index
                            );
            }

            bool isBroken() {
                return (srcBoard == boardDefaults::nullBoard);
            }
    };
}
#endif
