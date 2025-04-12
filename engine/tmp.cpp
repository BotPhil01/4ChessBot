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
    board::Board unequal = board::Board(boardDefaults::nullBoard, types::PieceType::PAWN);
    std::cout << "b == unequal: " << (b == unequal) << "\n";
    board::Board a;
    std::cout << "printing a\n";
    a.print();
    std::cout << "b == a: " << (b == a) << "\n";
    std::cout << "generating legal moves\n";
    b.generateLegalMoves(types::PieceColour::RED);
    std::cout << "b == a: " << (b == a) << "\n";
    types::PieceColour col = types::PieceColour::RED;

    engine::Engine eng = engine::Engine(a, col);
    std::cout << "printing a\n";
    a.print();

    std::cout << "after choose next movee";
    eng.chooseNextMove();
    std::cout << "eng.board == a: " << (eng.getBoard() == a) << "\n";
    // -> returns true
    std::cout << "b == a: " << (b == a) << "\n";
    std::cout << "printing b\n";
    b.print();
    std::cout << "printing a\n";
    a.print();
    // -> returns false
    // hence there is corruption of the eboard when running choosenextmove

    // 
    // std::vector<move::Move> movees = b.generateLegalMoves(col);
    // move::Move m = eng.chooseNextMove();
    // // TODO FIX:
    // // use:
    // // g++ -std=c++23 -g -O0 tmp.cpp && gdb ./a.out
    // // bits do not reset after above command
    // // permanent changes are made to the board in the bitboards that are not reversed
    // // problem is most likely in references and unplaying moves
    // // its 12:30 im going to bed
    // eng.evalMove(m);
}
