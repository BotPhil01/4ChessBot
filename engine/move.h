#include "bitboard.h"
#include "types.h"

#ifndef MOVE_H
#define MOVE_H
namespace move {
    // move is a container for squares and indices
    class Move {
        private:
            // TODO optimise later to use references
            bitboard::Bitboard &srcBoard;
            bitboard::Bitboard destBoard;
            bitboard::Bitboard specialBoard; // 1 byte
            // promotion
            types::PieceType toP; // 4 byte

        public:
            // 2 byte 
            int totalMoves; // holds how many other moves couldve been chosen at the time amongst this move
            unsigned short index = 255; // index in moves 


            constexpr Move(const Move &m) : 
            srcBoard(m.srcBoard),
            destBoard(m.destBoard),
            specialBoard(m.specialBoard),
            toP(m.toP),
            totalMoves(m.totalMoves),
            index(m.index)
            {}

            constexpr Move(Move &&m) : 
            srcBoard(m.srcBoard),
            destBoard(m.destBoard),
            specialBoard(m.specialBoard),
            toP(m.toP),
            totalMoves(m.totalMoves),
            index(m.index)
            {}
            
            constexpr Move& operator=(const Move& m) {
                srcBoard = m.srcBoard;
                destBoard = m.destBoard;
                specialBoard = m.specialBoard;
                toP = m.toP;
                totalMoves = m.totalMoves;
                index = m.index;
                return *this;
            }

            bool operator==(const Move&) const = default;

            // generic move
            Move(
                    bitboard::Bitboard &srcBoard,
                    const bitboard::Bitboard destBoard,
                    const bitboard::Bitboard specialBoard = boardDefaults::zeroed,
                    const types::PieceType tp = types::PieceType::EMPTY, 
                    const int total = 0, 
                    const int index = 0
            ) : 
                srcBoard(srcBoard),
                destBoard(destBoard),
                specialBoard(specialBoard),
                toP(tp), 
                totalMoves(total),
                index(index)
            {}

            constexpr types::PieceType promotionPiece() const {
                return  toP;
            }
    };
}
#endif
