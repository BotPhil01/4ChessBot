#include "bitboard.h"
#include"board.h"
#include"types.h"
#include "threading.h"
#include<cassert>
#include <functional>
#include "parser.h"

class EngineProcess {
#define EXIT 0
#define CLI 1
#define JS 2
    board::Board m_board = board::Board();
    threading::MasterThread m_bEngine = threading::MasterThread(m_board, types::PieceColour::BLUE);
    threading::MasterThread m_yEngine = threading::MasterThread(m_board, types::PieceColour::YELLOW);
    threading::MasterThread m_gEngine = threading::MasterThread(m_board, types::PieceColour::GREEN);
    std::array<const std::reference_wrapper<threading::MasterThread>, 3> engines = {
        std::ref(m_bEngine), std::ref(m_yEngine), std::ref(m_gEngine)
    };

    inline static bool s_terminating = false;

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
                    cleanup();
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
            for (int i = 0; i < engines.size(); i++) {
                move::Move engineMove = engines[i].get().chooseNextMove();
                if (engineMove.isBroken()) {
                    // blue is checkmate
                    const types::PieceColour colour = helper::playableColours[i + 1];
                    m_board.setPlayerCheckmate(colour);
                    return;
                }
                m_board.playMove(engineMove);
            }
            std::cout << "after playing all move\n";
        }

        void cleanup() {
            s_terminating = true;
            for (int i = 0; i < engines.size(); i++) {
                engines[i].get().terminate();
            }
        }
};

int main() {
    EngineProcess ep;
    int val = ep.start();
    std::cout << "terminating main\n";
    return val;
}
