#include "bitboard.h"
#include "board.h"
#include "boardDefaults.h"
#include "engine.h"
#include "move.h"
#include <vector>

#define ROWS 4
#define COLS 4
int main() {
    board::Board b;
    engine::Engine eng = engine::Engine(b, types::PieceColour::RED);
    move::Move m = eng.chooseNextMove();
    // TODO FIX:
    // use:
    // g++ -std=c++23 -g -O0 tmp.cpp && gdb ./a.out
    // bits do not reset after above command
    // permanent changes are made to the board in the bitboards that are not reversed
    // problem is most likely in references and unplaying moves
    // its 12:30 im going to bed
    eng.evalMove(m);
}
