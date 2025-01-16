// #include"../helper.h"
// #include"../types.h"
// #include"../playerData.h"
// #include"../board.h"
#include"../engine.h"

#include<vector>
#include<iostream>
#include<cassert>

// using namespace helper;
// using namespace types;
// using namespace std;
// using namespace player;
// using namespace board;
using namespace engine;

int main() {
    Engine re = Engine(PieceColour::RED);
    Engine be = Engine(PieceColour::BLUE);
    Engine ye = Engine(PieceColour::YELLOW);
    Engine ge = Engine(PieceColour::GREEN);
    
    auto m = re.chooseNextMove();
    assert(m.fromColour() == PieceColour::RED);
    assert(m.fromIndex() != 300);
    assert(m.toIndex() != 300);
    assert(re.evaluateBoard().first != PieceColour::NONE);
    return 1;
}