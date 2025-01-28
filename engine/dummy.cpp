#include"types.h"
#include"evaluator.h"
#include"engine.h"
#include"board.h"

#include<iostream>

int main() {
    eval::Evaluator eval;
    board::Board b;
    engine::Engine e = engine::Engine(b, types::PieceColour::RED, 2, false);
    std::cout << "eval before: " << eval.getEvaluation(b, b.getPlayers())[0] << "\n";
    b.printBoard();
    types::Move m = types::Move(53, 85, 0, types::PieceType::PAWN, types::PieceColour::RED, types::PieceType::EMPTY, types::PieceColour::NONE);
    b.playMove(m);
    std::cout << "----------------------------\n"; 
    b.printBoard();
    std::cout << "eval after manual move: " << eval.getEvaluation(b, b.getPlayers())[0] << "\n";
    b.unPlayMove();
    b.playMove(e.chooseNextMove());
    std::cout << "----------------------------\n"; 
    b.printBoard();
    std::cout << "eval after engine move: " << eval.getEvaluation(b, b.getPlayers())[0] << "\n";

    return 0;    
}