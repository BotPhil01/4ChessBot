#include "bitboard.h"
#include "board.h"
#include "engine.h"

#define ROWS 4
#define COLS 4
int main() {
    board::Board b;
    types::PieceColour col = types::PieceColour::RED;
    engine::Engine eng = engine::Engine(b, col);
    eng.chooseNextMove();
}
