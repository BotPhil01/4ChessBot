#include "bitboard.h"
#include "board.h"
#include "boardDefaults.h"
int main() {
    const std::array<bitboard::Bitboard, 6> boards = {
        boardDefaults::redPawns,
        boardDefaults::redRooks,
        boardDefaults::redKnights,
        boardDefaults::redBishops,
        boardDefaults::redQueens,
        boardDefaults::redKings
    };
    for (int i = 0; i < 6; i++) {
        types::PieceType type = helper::playablePieces[i];
        board::Board a(boards[i], type);
        std::vector<move::Move> moves = a.generateLegalMoves(helper::RED);
        a.print();
        std::cout << "legal moves: " << moves.size() << "\n";
    }
}
