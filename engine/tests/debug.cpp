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
    Player p = Player(PieceColour::RED);
    std::cout << "size of player " << sizeof(Player) << std::endl;
    std::cout << "size of player object " << sizeof(p) << std::endl;
    Board b = Board();
    std::cout << "size of board " << sizeof(Board) << std::endl;
    std::cout << "size of board object " << sizeof(b) << std::endl;
    Engine e = Engine(b, PieceColour::RED);
    std::cout << "size of engine " << sizeof(Engine) << std::endl;
    std::cout << "size of engine object " << sizeof(e) << std::endl;
    Move m = Move();
    std::cout << "size of move " << sizeof(Move) << std::endl;
    std::cout << "size of move object " << sizeof(m) << std::endl;
    
    std::cout << "size of colour " << sizeof(PieceColour) << std::endl;
    std::cout << "size of type " << sizeof(PieceType) << std::endl;
    std::cout << "size of bool " << sizeof(bool) << std::endl;
    return 0;
}