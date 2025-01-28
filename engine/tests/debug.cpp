#include"../board.h"
#include"../engine.h"
#include"../types.h"
#include"../playerData.h"
#include<iostream>


using namespace board;
using namespace engine;
using namespace types;
using namespace player;

int main() {
    Board b = Board();
    Engine e = Engine(b, PieceColour::RED, 10);
    e.chooseNextMove();
    return 0;
}