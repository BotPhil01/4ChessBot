#include "bitboard.h"
#include"engine.h"
#include"board.h"
#include"types.h"
#include<cassert>
#include<string>
#include<array>
using namespace engine;
using namespace std;
using namespace board;
using namespace types;
using namespace helper;
using namespace move;

class EngineProcess {
    private:
        enum class Options {
            CLIMODE,
            JSMODE,
            EXIT,
            ERROR
        };

        // for generating legal moves
        // for predicting moves
        
        Board board = Board();
        const Engine bEngine = Engine(board, PieceColour::BLUE);
        const Engine yEngine = Engine(board, PieceColour::YELLOW);
        const Engine gEngine = Engine(board, PieceColour::GREEN);

        std::array<const std::reference_wrapper<const Engine>, 3UL> engines = {cref(bEngine), cref(yEngine), cref(gEngine)};
        // TODO MAKE ARRAY OF REFERENCES
        // takes a javascript move and converts it to an engine move
        // difference is that js move has 0, 0 as top left cpp has bottom left
        // fromx|fromy|tox|toy
        
        
        // translates jcoordinates into c++ coords
        pair<int, int> fromJCoords(int x, int y) {
            return pair<int, int> {x + 1, (13 - y) + 2};
        }
        // converts 16x16 coords to javascript coords
        pair<int, int> toJCoords(pair <int, int> coords) {
            coords.first -= 1;
            coords.second -= 1;
            coords.second = 13 - coords.second;
            return coords;
        }

        // gives a js output for a cpp move
        void parseOutput(Move m) {
            pair<int, int> fromCoords = m.unarySrcBoard.to16RC();
            pair<int, int> toCoords =m.destBoard.to16RC();

            cout << "M" << fromCoords.first << "|" << fromCoords.second << "|" << toCoords.first << "|" << toCoords.second << "M" << endl;
        }

        // gives js output for a finished engine
        void parseOutput(Engine e) {
            // assert(board.isPlayerCheckmate(e.getColour()));            
            cout << "C" << colourToChar(e.getColour()) << "C" << endl;
        }

        void updateGameState(Move m) { // plays a move across the whole process
            board.playMove(m);
            return;
        }
        Move parseJsMove(string jMove) {
            jMove = jMove.substr(jMove.find_first_of("M") + 1, jMove.find_last_of("M"));
            // extract from string
            assert(jMove.size() > 3);
            vector<int> coordinates;
            size_t i = jMove.find('|', 0);
            while (i != jMove.npos) {
                coordinates.emplace_back(stoi(jMove.substr(0, i))); // place at end
                jMove = jMove.substr(i+1, jMove.size() - i);
                i = jMove.find('|', 0);
            }

            if (jMove.compare("") != 0) {
                coordinates.emplace_back(stoi(jMove));                
            }
            assert(coordinates.size() == 4);
            // reflect across x axis
            coordinates[1] = 13 - coordinates[1];
            coordinates[3] = 13 - coordinates[3];

            // add padding to convert from 14x14 to 16x18
            coordinates[0] += 1;
            coordinates[2] += 1;
            coordinates[1] += 2;
            coordinates[3] += 2;

            
            std::pair<int, int> from = std::pair(coordinates[0], coordinates[1]);
            std::pair<int, int> to = std::pair(coordinates[2], coordinates[3]);
            
            return board.indicesToMove(from, to);
        }

        // void parseSquare(string jSquare) {
        //     jSquare = jSquare.substr(jSquare.find_first_of("S") + 1, jSquare.find_last_of("S"));

        //     // in form [0-9]+|[0-9]+
        //     size_t t = jSquare.find("|");
        //     assert(t != jSquare.npos);
        //     pair<int, int> coords = fromJCoords(stoi(jSquare.substr(0, t)), stoi(jSquare.substr(t+1, jSquare.length())));
        //     boardIndex i = toIndex(coords.first, coords.second);
        //     // get indices
        //     // BUG HERE DOESNT GET THE CORRECT AMOUNT OF SQUARES AFTER MOVES:
        //     // M6|12|6|10M
        //     // M4|13|5|11M
        //     // M9|13|8|11M
        //     // M5|13|8|10M
        //     // S8|10S
        //     // INCORRECT OUTPUT:
        //     // Process has output data: <data>S9|9#10|8#S
        //     array<boardIndex, 41>  indices = board.genShift(i);
        //     // output the indices converted to jcoords
        //     string s = "S";
        //     for (unsigned int i = 0; i < indices.size() && indices[i] != 0; ++i) {
        //         boardIndex index = indices[i];
        //         pair<int, int> p = toJCoords(to16RC(index));
        //         s = s + to_string(p.first) + "|" + to_string(p.second) + "#";
        //     }
        //     s = s + "S";
        //     cout << s << endl;
        // }

        Options getProgramType(string input) {
            if (input.length() == 0) {
                return Options::ERROR;
            }
            if (input.compare("cli") == 0 || input.compare("1") == 0) {
                return Options::CLIMODE;
            }
            if (input.compare("js") == 0 || input.compare("2") == 0) {
                return Options::JSMODE;
            }
            if (input.compare("quit") == 0 || input.compare("exit") == 0 || input.compare("3") == 0) {
                return Options::EXIT;
            }
            return Options::ERROR;
        }

        void jsLoop() {
            string input;
            while (getline(cin, input)) {
                if (input.compare("quit") == 0) {
                    cout << "Comparison success exiting...\n"sv;
                    break;
                }
                assert(input.length() != 0);
                switch (input[0]) {
                    case 'S':
                        // parseSquare(input);
                        cout.flush();
                        continue;
                    case 'M':
                        updateGameState(parseJsMove(input));
                        break;
                    default:
                        cout << "Move is poorly formed\n"sv;
                        assert(false);
                }

                // run engines
                for (unsigned int i = 0; i < engines.size(); ++i) {
                    Engine e = engines[i].get();
                    Move m = e.chooseNextMove();

                    // if (m.fromIndex() == 300 || m.toIndex() == 300) {
                    if (m.destBoard == bitboard::nullBoard) {
                        parseOutput(e);
                    } else {
                        updateGameState(m);
                        parseOutput(m);
                    }
                }

                // check loss condition
                vector<Move> moves;
                moves = board.generateLegalMoves(PieceColour::RED);
                if (moves.size() == 0) {
                    cout << "L\n"sv;
                    return;
                }

                // check win condition;
                bool existsEnemy = false;
                existsEnemy = true;
                // for (Player p : board.getPlayers()) {
                //     if (p.colour() != PieceColour::RED && !p.isCheckmate()) {
                //         existsEnemy = true;
                //     }
                // }
                
                if (!existsEnemy) {
                    cout << "W\n"sv;
                    return;
                }
                
            }
        }

        // cli move comes in [a-n][1-14][a-n][1-14]
        // algebraic move
        Move parseCliMove(string_view input) {
            if (input.length() < 4) {
                return Move();
            }
            // break down the input into indices
            const uint8_t fromX = asc2PadCol(input[0]);
            input = input.substr(1, input.length());
            const unsigned char alphaIndex = input.find_first_of("abcdefghijklmopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"sv);
            assert(alphaIndex != input.npos);
            const uint8_t fromY = asc2PadRow(input.substr(0, alphaIndex));
            const uint8_t toX = asc2PadCol(input[alphaIndex]);
            const uint8_t toY = asc2PadRow(input.substr(alphaIndex + 1, input.length()));
            // return board.indicesToMove(toIndex(fromX, fromY), toIndex(toX, toY));
            return board.indicesToMove(std::pair(fromX, fromY), std::pair(toX, toY));
        }

        void cliLoop() {
            string input;
            std::cout << "Starting cli loop\nInput move algebraically eg g2g4 or quit or exit to exit the program\n"sv;
            board.print();
            while (getline(cin, input)) {
                if(getProgramType(input) == Options::EXIT) {
                    return;
                }
                Move m = parseCliMove(input);
                if (m.unarySrcBoard == bitboard::nullBoard) {
                    std::cout << "Incorrect input\nInput move algebraically eg: g2g4 or quit or exit to exit the program\n"sv;
                } else {
                    updateGameState(m);
                    for (unsigned char i = 0; i < engines.size(); ++i) {
                        Engine e = engines[i].get();
                        m = e.chooseNextMove(); 
                        if (m.unarySrcBoard.unary()) {
                            updateGameState(m);
                        }
                        std::cout << helper::colourToChar(e.getColour()) << "'s move:\n"sv;
                        board.print();
                    }
                    std::cout << "Awaiting input\n"sv;
                }
            }
        }

    public:
        int start() {
            string input;
            while (true) {
                cout << "Enter mode: 1.cli 2.js 3.exit\n";
                getline(cin, input);
                Options programType = getProgramType(input);
                switch(programType) {
                    case Options::JSMODE:
                        jsLoop();
                    case Options::CLIMODE:
                        cliLoop();
                    case Options::EXIT:
                        return 0;
                    default:
                        cout << "Invalid option\n";
                }

            }
        }
            

};

int main() {
    EngineProcess ep = EngineProcess();
    return ep.start();
}

