#include"types.h"
#include"helper.h"
#include"playerData.h"

#include<vector>
#include<string>

using namespace helper;
using namespace types;
using namespace player;
using namespace std;

#ifndef BOARD_H
#define BOARD_H

namespace board {
    // board is padded to 16x18
    class Board {
        private:
            std::vector<Square> boardVector; 
            
            PieceColour turn;
            std::vector<Move> moveVector; 
            std::vector<bool> playableColours; // r b y g

            Player redPlayer = Player(RED);
            Player bluePlayer = Player(BLUE);
            Player yellowPlayer = Player(YELLOW);
            Player greenPlayer = Player(GREEN);

            vector<reference_wrapper<Player &>> players;

            std::vector<Move> getMoveHistory() {
                return moveVector;
            }
            Square & getSquare(boardIndex r, boardIndex c) { // from 14x14
                short a1 = 33;
                return boardVector.at(r * PADDEDCOLS + (c + 1) + a1);
            }
            // Square & getSquareByType(COLUMN c, short r) {}
            Square & getSquarePadded(boardIndex r, boardIndex c) { // from 16x18
                return boardVector.at(r * 16 + c);
            }
            // helper function to query board if the piece at s can capture at t also checking if t exists (can be anything on board as long as diff colours)
            // note this is pseudo legal and hence should not be used as a definitive answer
            // assume src is a valid piece ie not empty
            // checks if target is on the board
            bool existsCapturable(boardIndex src, boardIndex trgt) {

                Square srcSqu = boardVector.at(src);
                Square trgtSqu = boardVector.at(trgt);
                return isOnBoard(trgt) && trgtSqu.colour() != srcSqu.colour() && trgtSqu.isAccessible();
            }
            // STRICTER THAN existsCapturable
            bool existsEnemyPiece(boardIndex src, boardIndex trgt) {
                Square trgtSqu = boardVector.at(trgt);
                Square srcSqu = boardVector.at(src);
                return isOnBoard(trgt) && trgtSqu.colour() != srcSqu.colour() && !trgtSqu.isEmpty() && trgtSqu.isAccessible();
            }            

            bool willPromote(boardIndex target, PieceColour playerCol) {
                switch (playerCol) {
                    case PieceColour::RED:
                        return target > 128;
                    case PieceColour::BLUE:
                        return target % 16 >= 8;
                    case PieceColour::YELLOW:
                        return target < 143;
                    case PieceColour::GREEN:
                        return target <= 7;
                    default:
                        return false;
                }
                return false;
            }

            // bulk creates moves from one index to a list of other indices
            // indices are assumed to be valid
            std::vector<Move> bulkCreateMove(boardIndex src, std::vector<boardIndex> trgts) {
                assert(indexOnBoard(src));
                std::vector<Move> out {};
                for (boardIndex t : trgts) {
                    
                    if (boardVector[src].type() == PieceType::PAWN && willPromote(t, boardVector[src].colour())) {
                        out.emplace_back(
                            src, t, 0,
                            boardVector[src].type(), boardVector[src].colour(), 
                            boardVector[t].type(), boardVector[t].colour(),
                            PieceType::QUEEN
                        );
                        continue;
                    } 
                    else if (isEnPeasant(src, t) || isCastling(src, t)){
                        out.emplace_back(
                            src, t, 0,
                            boardVector[src].type(), boardVector[src].colour(), 
                            boardVector[t].type(), boardVector[t].colour(),
                            true
                        );
                        continue;
                    }
                    out.emplace_back(
                        src, t, 0,
                        boardVector[src].type(), boardVector[src].colour(), 
                        boardVector[t].type(), boardVector[t].colour());
                }
                return out;
            }

            // generates pseudo legal moves
            // moves in returned vector have all except the totalMoves guaranteed to be filled
            std::vector<Move> generatePseudoLegalMoves(Player player) {
                std::vector<Move> out {};
                out.reserve(60);
                auto pieces = player.getPieces();
                int i = 0;
                for (set<boardIndex> &set : pieces) {
                    for (auto index : set)
                        switch (playablePieces[i]) {
                            case PieceType::PAWN:
                            out = concat(out, bulkCreateMove(index, pawnShift(index)));
                            break;
                        case PieceType::ROOK:
                            out = concat(out, bulkCreateMove(index, rookShift(index)));
                            break;
                        case PieceType::KNIGHT:
                            out = concat(out, bulkCreateMove(index, knightShift(index)));
                            break;
                        case PieceType::BISHOP:
                            out = concat(out, bulkCreateMove(i, bishopShift(index)));
                            break;
                        case PieceType::KING:
                            out = concat(out, bulkCreateMove(index, kingShift(index)));
                            break;
                        case PieceType::QUEEN:
                            out = concat(out, bulkCreateMove(index, queenShift(index)));
                            break;
                        default:
                            continue;
                        }

                }
                out.shrink_to_fit();
                return out;
            }

            // get colour's last move 
            // returns valid iterator if move exists otherwise returns .end() iterator
            std::vector<Move>::iterator getLastMoveIterator(PieceColour colour) {
                const size_t vs = moveVector.size();
                unsigned char min = std::min(vs, (const size_t) 4); 
                std::vector<Move>::iterator iterator = moveVector.end() - 1;
                for (int i = 0; i < min; ++i) {
                    if (vs - i > 0 && vs - i < moveVector.size()) {
                        Move m = moveVector[vs - i]; 
                        if (m.fromColour() == colour) {
                            return iterator - i;
                        }
                    }
                }
                return moveVector.end();
            }
            // check if a square is empty by index
            bool isEmpty(boardIndex i) {
                return boardVector[i].type() == PieceType::EMPTY;
            }

            bool isOnBoard(boardIndex i) {
                for (boardIndex c : cornerIndices) { // not in corners
                    if (c == i) {
                        return false;
                    }
                }
                boardIndex rem = i % 16;
                return rem != 0 && rem != 15 && i < 252 && i > 35;
            }

            bool hasMoved(boardIndex i) {
                auto c = boardVector[i].colour();
                Player p = players[indexFromColour(c)];
                return p.indexHasMoved(i);
            }
        
            std::vector<boardIndex> pawnShift(boardIndex index) {
                auto out = concat(pawnQuietShift(index), pawnCaptureShift(index));
                return out;
            }

            // Quiet move shift checks if on the board
            std::vector<boardIndex> pawnQuietShift(boardIndex index) {
                std::vector<boardIndex> out = {};
                boardIndex m = shiftOne(index, helper::getUp(turn));
                if (isEmpty(m) && isOnBoard(m)) {out.emplace_back(m);}
                if (!isPawnStart(index)) {
                    return out;
                }
                m = shiftOne(m, helper::getUp(turn));
                if (isEmpty(m) && isOnBoard(m)) {out.emplace_back(m);}
                return out;
            }

            // includes en peasant
            std::vector<boardIndex> pawnCaptureShift(boardIndex index) {
                // check if there exists a capturable piece beside 
                // TODO optimise later
                std::vector<boardIndex> out = std::vector<boardIndex> {}; // en peasant is unlikely 
                // regular captures
                boardIndex target = shiftOne(index, getUpRight(boardVector[index].colour()));
                bool upRightEmpty = isEmpty(target);
                if (existsEnemyPiece(index, target)) {
                    out.emplace_back(target);
                }

                target = shiftOne(index, getUpLeft(boardVector[index].colour())); // go to next possible capture
                bool upLeftEmpty = isEmpty(target);
                if (existsEnemyPiece(index, target)) {
                    out.emplace_back(target);
                }

                // en peasant
                target = shiftOne(index, getLeft(boardVector[index].colour()));
                PieceColour targetColour = boardVector[target].colour(); 
                std::vector<Move>::iterator lastMoveIterator = getLastMoveIterator(targetColour);
                if (upLeftEmpty && (lastMoveIterator != moveVector.end()) && (lastMoveIterator->toIndex() == target) && existsEnemyPiece(index, target)) { 
                    out.emplace_back(target);
                }

                target = shiftOne(index, getRight(boardVector[index].colour()));
                targetColour = boardVector[target].colour();
                lastMoveIterator = getLastMoveIterator(targetColour);
                if (upRightEmpty && (lastMoveIterator != moveVector.end()) && (lastMoveIterator->toIndex() == target) && existsEnemyPiece(index, target)) {
                    out.emplace_back(target);
                }
                return out;
            }

            std::vector<boardIndex> knightShift(boardIndex index) {
                std::vector<boardIndex> out {}; 
                boardIndex northSouth = shiftOne(index, Direction::NORTH);
                boardIndex eastWest = shiftOne(index, Direction::EAST);
                boardIndex diag = shiftOne(northSouth, Direction::NORTHEAST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = shiftOne(eastWest, Direction::NORTHEAST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = shiftOne(northSouth, Direction::NORTHWEST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = shiftOne(eastWest, Direction::NORTHWEST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                eastWest = shiftOne(index, Direction::WEST);
                northSouth = shiftOne(index, Direction::SOUTH);
                diag = shiftOne(northSouth, Direction::SOUTHEAST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = shiftOne(eastWest, Direction::SOUTHEAST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = shiftOne(northSouth, Direction::SOUTHWEST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = shiftOne(eastWest, Direction::SOUTHWEST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                return out;
            }

            // returns if the piece at i is a blocker
            bool isBlocker(boardIndex i) {
                return boardVector[i].type() != PieceType::EMPTY;
            }
            // returns the list of on board ray indices in a given direction from a given src
            std::vector<boardIndex> getRay(boardIndex src, Direction d) {
                std::vector<boardIndex> out {};
                boardIndex next = shiftOne(src, d);
                while (boardVector[next].type() == PieceType::EMPTY) {
                    out.emplace_back(next);
                    next = shiftOne(next, d);
                }
                // found blocker
                // add if capturable (blocker can be an out of bounds block piece)
                if (existsCapturable(src, next)) {
                    out.emplace_back(next);
                }
                return out;
            }
            std::vector<boardIndex> bishopShift(boardIndex src) {
                std::vector<boardIndex> out = getRay(src, Direction::NORTHEAST);
                concat(out, getRay(src, Direction::NORTHWEST));
                concat(out, getRay(src, Direction::SOUTHEAST));
                concat(out, getRay(src, Direction::SOUTHWEST));
                return out;
            }

            std::vector<boardIndex> rookShift(boardIndex src) {
                std::vector<boardIndex> out = getRay(src, Direction::NORTH);
                concat(out, getRay(src, Direction::EAST));
                concat(out, getRay(src, Direction::SOUTH));
                concat(out, getRay(src, Direction::WEST));
                return out;
            }

            std::vector<boardIndex> queenShift(boardIndex src) {
                return concat(rookShift(src), bishopShift(src));
            } 

            std::vector<boardIndex> kingShift(boardIndex src) {
                std::vector<Direction> directions = {Direction::NORTH, Direction::NORTHEAST, Direction::EAST, Direction::SOUTHEAST, Direction::SOUTH, Direction::SOUTHWEST, Direction::WEST, Direction::NORTHWEST};
                std::vector<boardIndex> out;
                boardIndex tmp;
                for (Direction d : directions) {
                    tmp = shiftOne(src, d);
                    if (existsCapturable(src, tmp)) {
                        out.emplace_back(tmp);
                    }
                }

                // calculate castling possibility
                if (hasMoved(src)) {
                    return out;
                }
                auto leftRay = getRay(src, getLeft(boardVector[src].colour()));
                auto rightRay = getRay(src, getRight(boardVector[src].colour())); 

                if (leftRay.size() > 0) {
                    auto endLeftIndex = leftRay.back();
                    if (boardVector[endLeftIndex].type() == PieceType::ROOK && hasMoved(endLeftIndex) == false) {
                        out.emplace_back(endLeftIndex);
                    }
                }
                if (rightRay.size() > 0) {
                    auto endRightIndex = rightRay.back();
                    if (boardVector[endRightIndex].type() == PieceType::ROOK && hasMoved(endRightIndex) == false) {
                        out.emplace_back(endRightIndex);
                    }
                }
                return out;
            }

            // indices passed into this should be verified that no other piece blocks the path between them
            bool isCastling(boardIndex src, boardIndex tgt) {
                Square s = boardVector[src];
                Square t = boardVector[tgt];
                if (!(s.type() == PieceType::KING && t.type() == PieceType::ROOK && s.colour() == t.colour())) {
                    return false;
                }

                if (hasMoved(src) || hasMoved(tgt)) {
                    return false;
                }
                return true;
            }

            

            // checks whether 2 indices constitute an enPeasant move
            bool isEnPeasant(boardIndex src, boardIndex trgt) {
                if (trgt != shiftOne(src, getLeft(boardVector[src].colour())) || 
                    trgt != shiftOne(src, getRight(boardVector[src].colour()))) {
                    return false;
                }
                if (boardVector[src].type() != PieceType::PAWN || boardVector[trgt].type() != PieceType::PAWN) {
                    return false;
                }
                return true;
            }

            std::vector<Square> getBoard() {
                return boardVector;
            }
            
            int getValue(PieceType t) {
                switch (t) {
                    case PieceType::PAWN:
                        return 1;
                    case PieceType::ROOK:
                        return 5;
                    case PieceType::BISHOP:
                        return 3;
                    case PieceType::KNIGHT:
                        return 3;
                    case PieceType::QUEEN:
                        return 9;
                    case PieceType::KING:
                        return 200;
                    default:
                        return 0;
                }
                return 0;
            }
        public:

            Board() {
                // construct to initial position
                for (short i = 0; i < 288; ++i) {
                    boardVector.emplace_back(generateSquare(i));
                }
                turn = initailTurn;
                moveVector = std::vector<Move> ();
                playableColours = {true, true, true, true};
                players.emplace_back(redPlayer);
                players.emplace_back(bluePlayer);
                players.emplace_back(yellowPlayer);
                players.emplace_back(greenPlayer);
            }
            
            PieceColour getCurrentTurn() {
                return turn;
            }

            // returns (c)'s last move if no move is found it returns the last move played
            Move & getLastMove(PieceColour c) {
                assert(moveVector.size() > 0);
                for (int i = 1; i < 5; i++) {
                    int index = moveVector.size() - i;
                    if (index < 0) {
                        break;
                    }
                    if (moveVector[index].fromColour() == c) {
                        return moveVector[index];
                    }
                }
                return moveVector[moveVector.size() - 1];
            } 

            void printBoard() {
                // TODO Change to unicode characters see https://www.chessprogramming.org/Pieces
                // algo is weird but prints correctly according to red
                // orientation in order NESW = YGRB
                for (short i = PADDEDROWS - 1; i >= 0; --i) {
                    for (int j = 0; j < PADDEDCOLS; ++j) {
                        PieceType type = boardVector[16 * i + j].type();
                        std::cout << PieceTypeToString(type).substr(0, 2) << " ";
                    }
                    std::cout << '\n';
                }
            }

            // asks whether c's king is in check in current board position
            bool isKingInCheck(Player player) {
                // BUG this does not adequately check whether the king is in check
                // knight just fucking takes the blue king
                // idea
                // get king index
                set<boardIndex> s = player.getPieces()[indexFromType(PieceType::KING)].get();
                auto node = s.extract(s.begin());
                assert(!node.empty());
                boardIndex kingIndex = node.value();

                // generate possible attacks

                // check ray attacks () and 
                std::vector<Direction> diags {Direction::NORTHEAST, Direction::SOUTHEAST, Direction::SOUTHWEST, Direction::NORTHWEST};
                std::vector<Direction> upDowns {Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST};
                for (Direction d : diags) {
                    // sliding piece check
                    auto ray = getRay(kingIndex, d);
                    if (ray.size() > 0) {
                        auto last = ray.back();
                        if (existsEnemyPiece(kingIndex, last) && (boardVector.at(last).type() == PieceType::BISHOP || boardVector.at(last).type() == PieceType::QUEEN)) {
                            return true;
                        }
                    }

                    // pawn check
                    auto diagIndex = shiftOne(kingIndex, d);
                    if (existsEnemyPiece(kingIndex, diagIndex) && (boardVector.at(diagIndex).type() == PieceType::PAWN)) {
                        // get the pawn shift
                        // if the result of the pawn shift is the kignIndex then return true 
                        auto pawnAttacks = pawnCaptureShift(diagIndex);
                        for (auto attack : pawnAttacks) {
                            if (attack == kingIndex) {
                                return true;
                            }
                        }
                    }
                }

                for (Direction d : upDowns) {
                    auto ray = getRay(kingIndex, d);
                    if (ray.size() > 0) {
                        auto last = ray.back();
                        if (existsEnemyPiece(kingIndex, last) && (boardVector.at(last).type() == PieceType::ROOK || boardVector.at(last).type() == PieceType::QUEEN)) {
                            return true;
                        }
                    }
                }

                // check knight
                auto knightRays = knightShift(kingIndex);
                assert(knightRays.size() < 9);
                for (boardIndex i : knightRays) {
                    if (existsEnemyPiece(kingIndex, i) && boardVector.at(i).type() == PieceType::KNIGHT) {
                        // std::cout << "knight ray found\n";
                        return true;
                    }
                }
                return false;
            }

            // generates the colours legal moves
            // returned moves can be guaranteed to be legal and all information except totalMoves to be correct
            std::vector<Move> generateLegalMoves(Player player) {
                std::vector<Move> out;
                auto moves = generatePseudoLegalMoves(player);
                for (auto m : moves) {
                    playMove(m);
                    if (!isKingInCheck(player)) {
                        out.emplace_back(m);
                    }
                    unPlayMove();
                }
                return out;
            }

            void playMove(Move m) {
                // runtime check that everything is working smoothly
                if ((boardVector[m.fromIndex()].type() != m.fromPiece()) || (boardVector[m.toIndex()].type() != m.promotionPiece())) {
                    printBoard();
                    std::string s = "Move is poorly formed: board square does not match move square data. Board type: " 
                    + std::string(PieceTypeToString(boardVector[m.fromIndex()].type())) + "(" + to_string(m.fromIndex()) + ")" 
                    + ", " + std::string(PieceTypeToString(m.fromPiece())) + "(" + to_string(m.fromIndex()) + ")" + "\n";
                    throw std::invalid_argument(s);
                }

                if (boardVector[m.fromIndex()].colour() != turn) {
                    std::string s = "It is not the given move's turn. Turn: " + std::string(PieceColourToString(turn)) 
                        + " colour: " + std::string(PieceColourToString(m.fromColour())) + " board piece colour: " 
                        + std::string(PieceColourToString(boardVector[m.fromIndex()].colour())) + " move: "
                        + moveToString(m) + "\n";
                    throw std::invalid_argument(s);
                }

                turn = getNextTurn(turn);

                // update player data
                players[indexFromColour(m.fromColour())].get().update(m);
                if (m.isCapture()) {
                    for (auto p : players) {
                        if (p.get().update(m) != PieceType::EMPTY) {
                            break;
                        }
                    }
                }
                
                // update board data
                // promotion
                if (m.isPromotion()) {
                    boardVector[m.toIndex()].setPiece(m.promotionPiece(), m.fromColour());
                    boardVector[m.fromIndex()].setPiece(PieceType::EMPTY, PieceColour::NONE);
                    
                    moveVector.emplace_back(m);
                    return;
                }

                if (m.isEnPeasant()) {
                    // NOTE enpeasant is encoded as src = pawn location and trgt = location of the adjascent pawn
                    boardIndex tmp = shiftOne(m.toIndex(), getUp(m.fromColour()));
                    boardVector[tmp].setPiece(m.fromPiece(), m.fromColour());
                    boardVector[m.fromIndex()].setPiece(PieceType::EMPTY, PieceColour::NONE);
                    boardVector[m.toIndex()].setPiece(PieceType::EMPTY, PieceColour::NONE);
                    moveVector.emplace_back(m);
                    return;
                }


                if (m.isCastling()) {
                    int dif = m.toIndex() - m.fromIndex();
                    int absDif = abs(dif);
                    if (absDif == 3 || absDif == 48) {
                        // queenside castle
                        boardVector[m.fromIndex() + (dif / 3 * 2)].setPiece(m.fromPiece(), m.fromColour());
                    } else {
                        // kingside castle
                        boardVector[m.fromIndex() + dif].setPiece(m.fromPiece(), m.fromColour());
                    }
                    boardVector[m.toIndex() - dif].setPiece(m.capturedPiece(), m.capturedColour());
                    boardVector[m.fromIndex()].setPiece(PieceType::EMPTY, PieceColour::NONE);
                    boardVector[m.toIndex()].setPiece(PieceType::EMPTY, PieceColour::NONE);

                    
                    moveVector.emplace_back(m);
                    return;
                }

                // quiet moves
                assert (indexOnBoard(m.fromIndex()) && indexOnBoard(m.toIndex()));
                boardVector[m.toIndex()].setPiece(m.fromPiece(), m.fromColour());
                boardVector[m.fromIndex()].setPiece(PieceType::EMPTY, PieceColour::NONE);
                moveVector.emplace_back(m);
            }

            // unplays the last played move
            void unPlayMove() {
                
                // handle board data
                Move lastMove = moveVector.back(); //retreive last elem 
                // clear special move squares (Squares altered outside of src and trgt)
                if (lastMove.isCastling()) {
                    // clear everything inbetween
                    Direction d = getDirection(lastMove.fromIndex(), lastMove.toIndex());
                    boardIndex tmp = lastMove.fromIndex();
                    while (tmp != lastMove.toIndex()) {
                        assert(isOnBoard(tmp));
                        boardVector[tmp].setPiece(PieceType::EMPTY, PieceColour::NONE);
                        tmp = shiftOne(tmp, d);
                    }
                }
                if (lastMove.isEnPeasant()) {
                    boardVector[shiftOne(lastMove.toIndex(), getUp(lastMove.fromColour()))].setPiece(PieceType::EMPTY, PieceColour::NONE);                    
                }
                boardVector[lastMove.toIndex()].setPiece(lastMove.capturedPiece(), lastMove.capturedColour()); //return data to state
                boardVector[lastMove.fromIndex()].setPiece(lastMove.fromPiece(), lastMove.fromColour()); 
                turn = getPrevTurn(turn);// reset turn counter
                moveVector.pop_back(); //remove from stack
                
                // handle player data
                // find the involved pieces last move 
                Move fromPrevMove;
                Move toPrevMove;
                for (Move m : moveVector) {
                    if (m.toIndex() == lastMove.fromIndex()) {
                        fromPrevMove = m;
                    }
                    if (m.toIndex() == lastMove.toIndex()) {
                        toPrevMove = m;
                    }
                }

                for (auto p : players) {
                    p.get().revert(lastMove, fromPrevMove, toPrevMove);
                }
            }
    
            // gets the current material value of a colour 
            float getMaterial(PieceColour c) {
                float res = 0;
                for (auto square : boardVector) {
                    if (square.colour() == c) {
                        res += getValue(square.type());
                    }
                }
                return res;
            }

            // returns the position of the given colour
            std::vector<std::pair<Square, boardIndex>> getPosition(PieceColour c) {
                std::vector<std::pair<Square, boardIndex>> res;
                for (int i = 0; i < boardVector.size(); ++i) {
                    Square square = boardVector[i];
                    if (square.colour() == c) {
                        res.emplace_back(std::pair<Square, boardIndex> {square, i});
                    }
                }    
                return res;            
            }   

            bool canPlay(PieceColour c) {
                switch(c) {
                    case PieceColour::RED:
                        return playableColours[0];
                    case PieceColour::BLUE:
                        return playableColours[0];
                    case PieceColour::YELLOW:
                        return playableColours[0];
                    case PieceColour::GREEN:
                        return playableColours[0];
                    default:
                        throw std::invalid_argument("PieceColour provided is not valid in board::canPlay(): " +  std::string(PieceColourToString(c)));
                }
                throw std::invalid_argument("Shouldn't have reached here in canPlay()");
            }
        

    };

    boardIndex toIndex( unsigned char c, unsigned char r) { // returns boardIndex from 16 * 18  coordinates
        return (r * 16 + c);
    }

    // translates a boardIndex to a padded row column pair
    std::pair<int, int> to16RC(boardIndex i) {
        int x = (i % 16);
        int y = (i - x) / 16;
        return std::pair(x,y);
    }
};
#endif