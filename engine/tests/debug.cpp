#include"../board.h"
#include"../engine.h"
#include"../types.h"

using namespace board;
using namespace engine;
using namespace types;

int main() {
    Move exp = Move(52, 68, 0, PieceType::PAWN, PieceColour::RED, PieceType::EMPTY, PieceColour::NONE);
    Board exp1 = Board();
    exp1.playMove(exp);
    return 1;
}