#include"engine.h"
#include"board.h"
#include"types.h"
#include<cassert>
#include<sstream>
#include<string>
#include<array>
using namespace engine;
using namespace std;
using namespace board;
using namespace types;
using namespace helper;
using namespace player;

class EngineProcess {
    private:
        char CLIMODE = 0;
        char JSMODE = 1;
        char EXIT = -1;

        // for generating legal moves
        // for predicting moves
        
        Board board = Board();
        Engine bEngine = Engine(board, PieceColour::BLUE);
        Engine yEngine = Engine(board, PieceColour::YELLOW);
        Engine gEngine = Engine(board, PieceColour::GREEN);

        std::array<std::reference_wrapper<Engine>, 3UL> engines = {ref(bEngine), ref(yEngine), ref(gEngine)};
        // TODO MAKE ARRAY OF REFERENCES
        // takes a javascript move and converts it to an engine move
        // difference is that js move has 0, 0 as top left cpp has bottom left
        // fromx|fromy|tox|toy
        
        Move parseJsMove(string jMove) {
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

            
            boardIndex from = toIndex(coordinates[0], coordinates[1]);
            boardIndex to = toIndex(coordinates[2], coordinates[3]);
            
            return board.indicesToMove(from, to);
        }

        // translates jcoordinates into c++ coords
        pair<int, int> fromJCoords(int x, int y) {
            return pair<int, int> {x + 1, (13 - y) + 2};
        }
        // converts 16x18 coords to javascript coords
        pair<int, int> toJCoords(pair <int, int> coords) {
            coords.first -= 1;
            coords.second -= 2;
            coords.second = 13 - coords.second;
            return coords;
        }

        // gives a js output for a cpp move
        string parseOutput(Move m) {
            stringstream stream;
            pair<int, int> fromCoords = toJCoords(to16RC(m.fromIndex()));
            pair<int, int> toCoords =  toJCoords(to16RC(m.toIndex()));
            
            stream << fromCoords.first << "|" << fromCoords.second << "|" << toCoords.first << "|" << toCoords.second << "#";
            return stream.str();
        }

        // gives js output for a finished engine
        string parseOutput(Engine e) {
            assert(board.isPlayerCheckmate(e.getColour()));
            stringstream stream;
            stream << colourToChar(e.getColour()) << "~";
            return stream.str();
        }

        void updateGameState(Move m) { // plays a move across the whole process
            board.playMove(m);
            return;
        }

        void parseSquare(string jSquare) {
            // in form [0-9]+|[0-9]+
            size_t t = jSquare.find("|");
            assert(t != jSquare.npos);
            pair<int, int> coords = fromJCoords(stoi(jSquare.substr(0, t)), stoi(jSquare.substr(t+1, jSquare.length())));
            boardIndex i = toIndex(coords.first, coords.second);
            // get indices
            vector<boardIndex> indices = board.genShift(i);
            // output the indices converted to jcoords
            string s = "@";
            for (boardIndex i : indices) {
                pair<int, int> p = toJCoords(to16RC(i));
                s = s + to_string(p.first) + "|" + to_string(p.second) + "#";
            }
            cout << s << endl;
        }

        int getProgramType(string input) {
            assert(input.length() > 0);
            if (input.compare("cli") == 0) {
                return CLIMODE;
            }
            if (input.compare("js") == 0) {
                return JSMODE;
            }
            if (input.compare("quit") == 0) {
                return EXIT;
            }
            return 10000;
        }

        void jsLoop() {
            string input;
            while (getline(cin, input)) {
                if (input.compare("quit") == 0) {
                    cout << "Comparison success exiting..." << "\n";
                    break;
                }
                assert(input.length() != 0);
                if (input[0] == '@') {
                    parseSquare(input.substr(1, input.length()));
                    cout.flush();
                    cout.flush();
                    continue;
                }
                Move m = parseJsMove(input);
                updateGameState(m);
                for (unsigned int i = 0; i < engines.size(); ++i) {
                    Engine e = engines[i].get();
                    Move m = e.chooseNextMove();
                    if (m.fromIndex() == 300 || m.toIndex() == 300) {
                        cout << parseOutput(bEngine) << endl;
                    } else {
                        updateGameState(m);
                        cout << parseOutput(m);
                    }
                }

                // check if red player has legal moves
                vector<unique_ptr<Move>> moves;
                board.generateLegalMoves(PieceColour::RED, moves);
                if (moves.size() == 0) {
                    cout << colourToChar(PieceColour::RED) << "!" << endl;
                }
                
                cout.flush();
            }
            return;
        }

        // cli move comes in [a-n][1-14][a-n][1-14]
        // algebraic move
        Move parseCliMove(string input) {
            // break down the 
            unsigned char fromX = asciiToCol(input[0]) + 1;
            input = input.substr(1, input.length());
            unsigned char alphaIndex = input.find_first_of("abcdefghijklmopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
            assert(alphaIndex != input.npos);
            unsigned char fromY = atoi(input.substr(0, alphaIndex).c_str()) + 1; 
            unsigned char toX = asciiToCol(input[alphaIndex]) + 1;
            unsigned char toY = atoi(input.substr(alphaIndex + 1, input.length()).c_str()) + 1;
            return board.indicesToMove(toIndex(fromX, fromY), toIndex(toX, toY));
        }

        void cliLoop() {
            string input;
            board.printBoard();
            while (getline(cin, input)) {
                if(getProgramType(input) == EXIT) {
                    return;
                }
                Move m = parseCliMove(input);
                updateGameState(m);
                for (unsigned char i = 0; i < engines.size(); ++i) {
                    Engine e = engines[i].get();
                    m = e.chooseNextMove(); 
                    if (m.fromIndex() == 300 || m.toIndex() == 300) {
                        cout << parseOutput(e) << endl;
                    } else {
                        updateGameState(m);
                        cout << parseOutput(m) << endl;
                    }
                    cout.flush();
                }
                board.printBoard();
            }
        }

    public:
        EngineProcess() {
            
        }
        int start() {
            string input;
            cout << "enter mode: 1.cli 2.js 3.exit" << endl;
            getline(cin, input);
            int programType = getProgramType(input);
            if (programType ==  JSMODE) {
                jsLoop();
                return 0;
            }
            if (programType == CLIMODE) {
                cliLoop();
                return 0;
            }
            if (programType == EXIT) {
                return 0;
            }
            return 0;
        }
            

};

int main() {
    EngineProcess ep = EngineProcess();
    return ep.start();
}

