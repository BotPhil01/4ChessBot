#include"engine.h"
#include"board.h"
#include<cassert>
#include<sstream>
using namespace engine;
using namespace std;
using namespace board;

class EngineProcess {
    private:
        // for generating legal moves
        // for predicting moves
        
        Board rBoard = Board();
        Engine bEngine = Engine(rBoard, PieceColour::BLUE);
        Engine yEngine = Engine(rBoard, PieceColour::YELLOW);
        Engine gEngine = Engine(rBoard, PieceColour::GREEN);
        // takes a javascript move and converts it to an engine move
        // difference is that js move has 0, 0 as top left cpp has bottom left
        // fromx|fromy|tox|toy
        
        Move parseInput(string jMove) {
            // extract from string
            assert(jMove.size() > 6);
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
            
            return rBoard.indicesToMove(from, to);
        }

        // converts 16x18 coords to javascript coords
        pair<int, int> toJCoords(pair <int, int> coords) {
            coords.first -= 1;
            coords.second -= 2;
            coords.second = 13 - coords.second;
            return coords;
        }

        // gives a js output for a cppp move
        string parseOutput(Move m) {
            stringstream stream;
            pair<int, int> fromCoords = toJCoords(to16RC(m.fromIndex()));
            pair<int, int> toCoords =  toJCoords(to16RC(m.toIndex()));
            
            stream << fromCoords.first << "|" << fromCoords.second << "|" << toCoords.first << "|" << toCoords.second << "#";
            return stream.str();
        }

        // gives js output for a finished engine
        string parseOutput(Engine e) {
            assert(e.hasFinished);
            stringstream stream;
            stream << PieceColourToString(e.getColour()) << "#";
            return stream.str();
        }

        void updateGameState(Move m) { // plays a move across the whole process
            rBoard.playMove(m);
            // bEngine.playMove(m);
            // yEngine.playMove(m);
            // gEngine.playMove(m);
            return;
        }


    public:
        EngineProcess() {
            
        }
        bool startLoop() {
            string input;
            while (getline(cin, input)) {
                if (input.compare("quit") == 0) {
                    cout << "Comparison success exiting..." << "\n";
                    break;
                }
                Move m = parseInput(input);
                updateGameState(m);
                if (!bEngine.hasFinished) {
                    Move bm = bEngine.chooseNextMove();
                    if (bm.fromIndex() == 300 || bm.toIndex() == 300) {
                        cout << parseOutput(bEngine) << endl;
                    } else {
                        updateGameState(bm); // here yEngine has correct turn
                        cout << parseOutput(bm) << endl;
                    }
                } else {
                    cout << parseOutput(bEngine) << endl;
                }
                if (!yEngine.hasFinished) {
                    Move ym = yEngine.chooseNextMove(); // here
                    if (ym.fromIndex() == 300 || ym.toIndex() == 300) {
                        cout << parseOutput(yEngine) << endl;
                    } else {
                        updateGameState(ym);
                        cout << parseOutput(ym) << endl;
                    }
                } else {
                    cout << parseOutput(yEngine) << endl;
                }
                if (!gEngine.hasFinished) {
                    Move gm = gEngine.chooseNextMove();
                    if (gm.fromIndex() == 300 || gm.toIndex() == 300) {
                        cout << parseOutput(gEngine) << endl;
                    } else {
                        updateGameState(gm);
                        cout << parseOutput(gm) << endl;
                    }
                } else {
                    cout << parseOutput(gEngine) << endl;
                }
                if(bEngine.hasFinished && yEngine.hasFinished && gEngine.hasFinished) {
                    cout << "^" << endl;
                }
                
                cout.flush();
            }
            return 0;
        }

};

int main() {
    EngineProcess ep = EngineProcess();
    return ep.startLoop();
}

