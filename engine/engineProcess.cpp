#include "bitboard.h"
#include"board.h"
#include"types.h"
#include "threading.h"
#include<cassert>
#include "parser.h"

class EngineProcess {
#define EXIT 0
#define CLI 1
#define JS 2
        
    board::Board m_board = board::Board();
    threading::MasterThread m_bEngine = threading::MasterThread(m_board, types::PieceColour::BLUE);
    threading::MasterThread m_yEngine = threading::MasterThread(m_board, types::PieceColour::YELLOW);
    threading::MasterThread m_gEngine = threading::MasterThread(m_board, types::PieceColour::GREEN);

    parser::Parser m_parser = parser::Parser(m_board);
    public:
        EngineProcess() 
        {}
        int start() {
            std::cout << "Starting program\n";

            start:
            std::cout << "Enter mode:\n0: exit\n1: cli\n2: js\n";
            int mode;
            std::cin >> mode;
            switch (mode) {
                case EXIT:
                    return 0;
                case CLI:
                    return startCli();
                    break;
                default:
                    std::cout << "Invalid input try again\n";
                    goto start;
            }
        }
    private:
        int startCli() {
            std::string input = "";
            while (true) {
                m_board.print();
                std::cout << "input ascii move\n";
                while(input.compare("") == 0) {
                    getline(std::cin, input);
                }
                if (input.compare("exit") == 0) {
                    return 0;
                }
                move::Move parsed = m_parser.parseCliMove(input);
                if (parsed.isBroken()) {
                    input = "";
                    std::cout << "bad input try again\n";
                    continue;
                }
                input = "";
                m_board.playMove(parsed);
                queryEngines();
            }

            return 1;
        }

        void queryEngines() {
            move::Move blueMove = m_bEngine.chooseNextMove();
            if (blueMove.isBroken()) {
                // blue is checkmate
                m_board.setPlayerCheckmate(types::PieceColour::BLUE);
                return;
            }
            m_board.playMove(blueMove);
            std::cout << "after playing blue move\n";
        }
};

int main() {
    EngineProcess ep;
    int val = ep.start();
    std::cout << "terminating main\n";
    return val;
}
