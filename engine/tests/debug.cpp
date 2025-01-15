#include"../helper.h"
#include"../types.h"
#include"../playerData.h"
#include<vector>
#include<iostream>
#include<cassert>

using namespace helper;
using namespace types;
using namespace std;
using namespace player;

int main() {
    Move m = Move(52, 68, 100, PieceType::PAWN, PieceColour::RED, PieceType::EMPTY, PieceColour::NONE);
    Move em = Move();
    Player r = Player(PieceColour::RED);
    r.update(m);
    r.revert(m, em, em);
    assert(1 == 2);
    return 1;
}