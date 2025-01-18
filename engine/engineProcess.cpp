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

class EngineProcess {
    private:
        // for generating legal moves
        // for predicting moves
        
        Board rBoard = Board();
        Engine bEngine = Engine(rBoard, types::PieceColour::BLUE);
        Engine yEngine = Engine(rBoard, types::PieceColour::YELLOW);
        Engine gEngine = Engine(rBoard, types::PieceColour::GREEN);
        // takes a javascript move and converts it to an engine move
        // difference is that js move has 0, 0 as top left cpp has bottom left
        // fromx|fromy|tox|toy
        
        types::Move parseMove(string jMove) {
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

            
            types::boardIndex from = helper::toIndex(coordinates[0], coordinates[1]);
            types::boardIndex to = helper::toIndex(coordinates[2], coordinates[3]);
            
            return rBoard.indicesToMove(from, to);
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

        // gives a js output for a cppp move
        string parseOutput(types::Move m) {
            stringstream stream;
            pair<int, int> fromCoords = toJCoords(helper::to16RC(m.fromIndex()));
            pair<int, int> toCoords =  toJCoords(helper::to16RC(m.toIndex()));
            
            stream << fromCoords.first << "|" << fromCoords.second << "|" << toCoords.first << "|" << toCoords.second << "#";
            return stream.str();
        }

        // gives js output for a finished engine
        string parseOutput(Engine e) {
            assert(rBoard.isPlayerCheckmate(e.getColour()));
            stringstream stream;
            stream << helper::PieceColourToString(e.getColour()) << "#";
            return stream.str();
        }

        void updateGameState(types::Move m) { // plays a move across the whole process
            rBoard.playMove(m);
            return;
        }

        void parseSquare(string jSquare) {
            // in form [0-9]+|[0-9]+
            unsigned int t = jSquare.find("|");
            assert(t != jSquare.npos);
            pair<int, int> coords = fromJCoords(stoi(jSquare.substr(0, t)), stoi(jSquare.substr(t+1, jSquare.length())));
            types::boardIndex i = helper::toIndex(coords.first, coords.second);
            // get indices
            vector<types::boardIndex> indices = rBoard.genShift(i);
            // output the indices converted to jcoords
            string s = "@";
            for (types::boardIndex i : indices) {
                pair<int, int> p = toJCoords(helper::to16RC(i));
                s = s + to_string(p.first) + "|" + to_string(p.second) + "#";
            }
            cout << s << endl;
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
                assert(input.length() != 0);
                if (input[0] == '@') {
                    parseSquare(input.substr(1, input.length()));
                    cout.flush();
                    continue;
                }
                types::Move m = parseMove(input);
                updateGameState(m);
                types::Move bm = bEngine.chooseNextMove();
                if (bm.fromIndex() == 300 || bm.toIndex() == 300) {
                    cout << parseOutput(bEngine) << endl;
                } else {
                    updateGameState(bm); // here yEngine has correct turn
                    cout << parseOutput(bm) << endl;
                }
                types::Move ym = yEngine.chooseNextMove(); // here
                if (ym.fromIndex() == 300 || ym.toIndex() == 300) {
                    cout << parseOutput(yEngine) << endl;
                } else {
                    updateGameState(ym);
                    cout << parseOutput(ym) << endl;
                }
        
                types::Move gm = gEngine.chooseNextMove();
                if (gm.fromIndex() == 300 || gm.toIndex() == 300) {
                    cout << parseOutput(gEngine) << endl;
                } else {
                    updateGameState(gm);
                    cout << parseOutput(gm) << endl;
                }
                
                // if(bEngine.hasFinished && yEngine.hasFinished && gEngine.hasFinished) {
                //     cout << "^" << endl;
                // }
                
                cout.flush();
            }
            return 0;
        }

};

int main() {
    EngineProcess ep = EngineProcess();
    return ep.startLoop();
}

