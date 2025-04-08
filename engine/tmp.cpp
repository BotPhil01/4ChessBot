#include "bitboard.h"
#include "board.h"

#define ROWS 4
#define COLS 4
int main() {
    board::Board b = board::Board();
    std::vector<move::Move> moves = b.generatePseudoLegalMoves(types::PieceColour::RED);
    std::cout << moves.size();
}
