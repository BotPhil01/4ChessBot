#include "bitboard.h"
#include "board.h"
#include "timer.h"
#include "threading.h"
#include "move.h"

#define ROWS 4
#define COLS 4
int main() {
    board::Board b;
    types::PieceColour colour = types::PieceColour::RED;
    threading::MasterThread master = threading::MasterThread(b, colour);
    move::Move m = master.chooseNextMove();
}
