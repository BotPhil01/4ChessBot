#include"engine.h"
#include"board.h"
#include<cassert>
#include<sstream>
using namespace engine;
using namespace std;
using namespace board;

class EngineProcess {
    private:
        Board rBoard; // for generating legal moves
        Engine bEngine; // for predicting moves
        Engine yEngine;
        Engine gEngine;
        // takes a javascript move and converts it to an engine move
        // difference is that js move has 0, 0 as top left cpp has bottom left
        Move parseInput(string jMove) {
            // extract from string
            // fromx|fromy|tox|toy
            assert(jMove.size() > 8);
            vector<int> coordinates;
            size_t i = jMove.find('|', 0);
            while (i != jMove.npos) {
                // cout << "substring: " << jMove.substr(0, i) << "\n";
                coordinates.emplace_back(stoi(jMove.substr(0, i))); // place at end
                // cout << "jMove" << jMove << "\n";
                jMove = jMove.substr(i+1, jMove.size() - i);
                i = jMove.find('|', 0);
            }

            if (jMove.compare("") != 0) {
                coordinates.emplace_back(stoi(jMove));                
            }
            assert(coordinates.size() == 4);
            // for (auto coord : coordinates) {
            //     std::cout << "coord: " << coord << "\n";
            // }
            // reflect across x axis
            coordinates[1] = 13 - coordinates[1];
            coordinates[3] = 13 - coordinates[3];

            // add padding to convert from 14x14 to 16x18
            coordinates[0] += 1;
            coordinates[2] += 1;
            coordinates[1] += 2;
            coordinates[3] += 2;

            // cout << "coords post adjustment: \n";
            // cout << "x: " << coordinates[0] << "\n";
            // cout << "y: " << coordinates[1] << "\n";
            // cout << "x: " << coordinates[2] << "\n";
            // cout << "y: " << coordinates[3] << "\n";
            

            boardIndex from = toIndex(coordinates[0], coordinates[1]);
            boardIndex to = toIndex(coordinates[2], coordinates[3]);
            // cout << "from index: " << from << "\n";
            // cout << "to Index: " << to << "\n";
            
            Move m = rBoard.createMove(from, to);
        
            return m;
        }

        // converts 16x18 coords to javascript coords
        pair<int, int> toJCoords(pair <int, int> coords) {
            // std::cout << "toJCoords from coords: " << coords.first << "," << coords.second << "\n";
            coords.first -= 1;
            coords.second -= 2;
            coords.second = 13 - coords.second;
            return coords;
        }

        // translates a cpp move into a js move
        string parseOutput(Move m) {
            // std::cout << "Output Move: " << moveToString(m) << "\n";
            stringstream stream;
            pair<int, int> fromCoords = toJCoords(to16RC(m.fromIndex));
            pair<int, int> toCoords =  toJCoords(to16RC(m.toIndex));
            
            stream << fromCoords.first << "|" << fromCoords.second << "|" << toCoords.first << "|" << toCoords.second << "\n";
            return stream.str();
        }

        void updateGameState(Move m) { // plays a move across the whole process
            rBoard.playMove(m);
            bEngine.playMove(m);
            yEngine.playMove(m);
            gEngine.playMove(m);
            return;
        }


    public:
        EngineProcess() {
            rBoard = Board();
            bEngine = Engine(PieceColour::BLUE);
            bEngine = Engine(PieceColour::BLUE);
            bEngine = Engine(PieceColour::BLUE);
        }
        bool startLoop() {
            string input;
            while (getline(cin, input)) {
                // cout << "Input received" << "\n";
                if (input.compare("quit") == 0) {
                    cout << "Comparison success exiting..." << "\n";
                    break;
                }
                // cout << "Comparison failed continuing...\n" ;
                Move m = parseInput(input);
                updateGameState(m);
                Move bm = bEngine.chooseNextMove();
                updateGameState(bm);
                Move ym = yEngine.chooseNextMove();
                updateGameState(ym);
                Move gm = gEngine.chooseNextMove();
                updateGameState(gm);
                //write to stdout the string translation of the moves
                cout << parseOutput(bm) << parseOutput(ym) << parseOutput(gm) << endl;
                cout.flush();
            }
            return 0;
        }

};

int main() {
    EngineProcess ep;
    return ep.startLoop();
}

