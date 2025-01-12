#include"helper.h"
#include"types.h"

#include<vector>
#ifndef BOARDDATA_H
#define BOARDDATA_H

// implements with piece lists instead of board vector

namespace data {
    using namespace std;
    using namespace helper;
    using namespace types;
    using Colour = PieceColour;
    class boardData {
        private:
            // piece lists
            vector<Piece> redPieces;
            vector<Piece> bluePieces;
            vector<Piece> yellowPieces;
            vector<Piece> greenPieces;
            // colours that have the ability to play according to the last move
            vector<Colour> remainingColours; 

            // generate a colours starting pieces
            vector<Piece> genStart(Colour c) {

                return vector<Piece> {genPawns(c), genRooks(c), genBishop
            }

        public:
            boardData() {
                redPieces = genStart(RED);
                bluePieces = genStart(BLUE);
                yellowPieces = genStart(YELLOW);
                greenPieces = genStart(GREEN);
                remainingColours = vector<Colour> {RED, BLUE, YELLOW, GREEN};
            }

    }
}


#endif