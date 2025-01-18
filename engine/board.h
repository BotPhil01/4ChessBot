#include"types.h"
#include"helper.h"
#include"playerData.h"

#include<vector>
#include<string>

#ifndef BOARD_H
#define BOARD_H

namespace board {
    // board is padded to 16x18
    class Board {
        std::vector<types::Square> boardVector; 
        
        types::PieceColour turn;
        std::vector<types::Move> moveVector; 
        std::vector<bool> playableColours; // r b y g

        player::Player redPlayer = player::Player(types::PieceColour::RED);
        player::Player bluePlayer = player::Player(types::PieceColour::BLUE);
        player::Player yellowPlayer = player::Player(types::PieceColour::YELLOW);
        player::Player greenPlayer = player::Player(types::PieceColour::GREEN);

        std::vector<std::reference_wrapper<player::Player>> players;

        private:

            std::vector<types::Move> getMoveHistory() {
                return moveVector;
            }
            types::Square & getSquare(types::boardIndex r, types::boardIndex c) { // from 14x14
                short a1 = 33;
                return boardVector.at(r * helper::PADDEDCOLS + (c + 1) + a1);
            }
            // types::Square & getSquareByType(COLUMN c, short r) {}
            types::Square & getSquarePadded(types::boardIndex r, types::boardIndex c) { // from 16x18
                return boardVector.at(r * 16 + c);
            }
            // helper function to query board if the piece at s can capture at t also checking if t exists (can be anything on board as long as diff colours)
            // note this is pseudo legal and hence should not be used as a definitive answer
            // assume src is a valid piece ie not empty
            // checks if target is on the board
            bool existsCapturable(types::boardIndex src, types::boardIndex trgt) {

                types::Square srcSqu = boardVector.at(src);
                types::Square trgtSqu = boardVector.at(trgt);
                return isOnBoard(trgt) && trgtSqu.colour() != srcSqu.colour() && trgtSqu.isAccessible();
            }
            // STRICTER THAN existsCapturable
            bool existsEnemyPiece(types::boardIndex src, types::boardIndex trgt) {
                types::Square trgtSqu = boardVector.at(trgt);
                types::Square srcSqu = boardVector.at(src);
                return isOnBoard(trgt) && trgtSqu.colour() != srcSqu.colour() && !trgtSqu.isEmpty() && trgtSqu.isAccessible();
            }            

            bool isPromotion(types::boardIndex from, types::boardIndex to) {
                if (boardVector[from].type() != types::PieceType::PAWN) {
                    return false;
                }
                switch (boardVector[from].colour()) {
                    case types::PieceColour::RED:
                        return to > 128;
                    case types::PieceColour::BLUE:
                        return to % 16 >= 8;
                    case types::PieceColour::YELLOW:
                        return to < 143;
                    case types::PieceColour::GREEN:
                        return to <= 7;
                    default:
                        return false;
                }
                return false;
            }

            // bulk creates moves from one index to a list of other indices
            // indices are assumed to be valid
            std::vector<types::Move> bulkCreateMove(types::boardIndex src, std::vector<types::boardIndex> trgts) {
                assert(helper::indexOnBoard(src));
                std::vector<types::Move> out {};
                for (types::boardIndex t : trgts) {
                    
                    if (boardVector[src].type() == types::PieceType::PAWN && isPromotion(src, t)) {
                        out.emplace_back(
                            src, t, 0,
                            boardVector[src].type(), boardVector[src].colour(), 
                            boardVector[t].type(), boardVector[t].colour(),
                            types::PieceType::QUEEN
                        );
                        continue;
                    } 
                    else if (isEnPeasant(src, t) || isCastling(src, t)){
                        out.emplace_back(
                            src, t, 0,
                            boardVector[src].type(), boardVector[src].colour(), 
                            boardVector[t].type(), boardVector[t].colour(),
                            types::PieceType::EMPTY, true
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
            // moves in returned std::vector have all except the totalMoves guaranteed to be filled
            std::vector<types::Move> generatePseudoLegalMoves(types::PieceColour c) {
                player::Player &player = players[helper::indexFromColour(c)].get();
                std::vector<types::Move> out {};
                out.reserve(60);
                std::vector<std::reference_wrapper<std::set<types::boardIndex>>> pieces = player.getPieces();
                for (unsigned int i = 0; i < pieces.size(); ++i) {
                    std::set<types::boardIndex> &s = pieces[i].get();
                    for (auto index : s)
                        switch (helper::playablePieces[i]) {
                            case types::PieceType::PAWN:
                            helper::concat(out, bulkCreateMove(index, pawnShift(index)));
                            break;
                        case types::PieceType::ROOK:
                            helper::concat(out, bulkCreateMove(index, rookShift(index)));
                            break;
                        case types::PieceType::KNIGHT:
                            helper::concat(out, bulkCreateMove(index, knightShift(index)));
                            break;
                        case types::PieceType::BISHOP:
                            helper::concat(out, bulkCreateMove(index, bishopShift(index)));
                            break;
                        case types::PieceType::KING:
                            helper::concat(out, bulkCreateMove(index, kingShift(index)));
                            break;
                        case types::PieceType::QUEEN:
                            helper::concat(out, bulkCreateMove(index, queenShift(index)));
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
            std::vector<types::Move>::iterator getLastMoveIterator(types::PieceColour colour) {
                const size_t vs = moveVector.size();
                unsigned char min = std::min(vs, (const size_t) 4); 
                std::vector<types::Move>::iterator iterator = moveVector.end() - 1;
                for (int i = 0; i < min; ++i) {
                    if (vs - i > 0 && vs - i < moveVector.size()) {
                        types::Move m = moveVector[vs - i]; 
                        if (m.fromColour() == colour) {
                            return iterator - i;
                        }
                    }
                }
                return moveVector.end();
            }
            // check if a square is empty by index
            bool isEmpty(types::boardIndex i) {
                return boardVector[i].type() == types::PieceType::EMPTY;
            }

            bool isOnBoard(types::boardIndex i) {
                for (types::boardIndex c : helper::cornerIndices) { // not in corners
                    if (c == i) {
                        return false;
                    }
                }
                types::boardIndex rem = i % 16;
                return rem != 0 && rem != 15 && i < 252 && i > 35;
            }

            bool hasMoved(types::boardIndex i) {
                auto c = boardVector[i].colour();
                player::Player p = players[helper::indexFromColour(c)];
                return p.indexHasMoved(i);
            }
        
            std::vector<types::boardIndex> pawnShift(types::boardIndex index) {
                std::vector<types::boardIndex> out = pawnQuietShift(index);
                helper::concat(out, pawnCaptureShift(index));
                return out;
            }

            // Quiet move shift checks if on the board
            std::vector<types::boardIndex> pawnQuietShift(types::boardIndex index) {
                std::vector<types::boardIndex> out = {};
                types::boardIndex m = helper::shiftOne(index, helper::getUp(turn));
                if (isEmpty(m) && isOnBoard(m)) {out.emplace_back(m);}
                if (!helper::isPawnStart(index)) {
                    return out;
                }
                m = helper::shiftOne(m, helper::getUp(turn));
                if (isEmpty(m) && isOnBoard(m)) {out.emplace_back(m);}
                return out;
            }

            // includes en peasant
            std::vector<types::boardIndex> pawnCaptureShift(types::boardIndex index) {
                // check if there exists a capturable piece beside 
                // TODO optimise later
                std::vector<types::boardIndex> out = std::vector<types::boardIndex> {}; // en peasant is unlikely 
                // regular captures
                types::boardIndex target = helper::shiftOne(index, helper::getUpRight(boardVector[index].colour()));
                bool upRightEmpty = isEmpty(target);
                if (existsEnemyPiece(index, target)) {
                    out.emplace_back(target);
                }

                target = helper::shiftOne(index, helper::getUpLeft(boardVector[index].colour())); // go to next possible capture
                bool upLeftEmpty = isEmpty(target);
                if (existsEnemyPiece(index, target)) {
                    out.emplace_back(target);
                }

                // en peasant
                target = helper::shiftOne(index, helper::getLeft(boardVector[index].colour()));
                types::PieceColour targetColour = boardVector[target].colour(); 
                std::vector<types::Move>::iterator lastMoveIterator = getLastMoveIterator(targetColour);
                if (upLeftEmpty && (lastMoveIterator != moveVector.end()) && (lastMoveIterator->toIndex() == target) && existsEnemyPiece(index, target)) { 
                    out.emplace_back(target);
                }

                target = helper::shiftOne(index, helper::getRight(boardVector[index].colour()));
                targetColour = boardVector[target].colour();
                lastMoveIterator = getLastMoveIterator(targetColour);
                if (upRightEmpty && (lastMoveIterator != moveVector.end()) && (lastMoveIterator->toIndex() == target) && existsEnemyPiece(index, target)) {
                    out.emplace_back(target);
                }
                return out;
            }

            std::vector<types::boardIndex> knightShift(types::boardIndex index) {
                std::vector<types::boardIndex> out {}; 
                types::boardIndex northSouth = helper::shiftOne(index, types::Direction::NORTH);
                types::boardIndex eastWest = helper::shiftOne(index, types::Direction::EAST);
                types::boardIndex diag = helper::shiftOne(northSouth, types::Direction::NORTHEAST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = helper::shiftOne(eastWest, types::Direction::NORTHEAST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = helper::shiftOne(northSouth, types::Direction::NORTHWEST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = helper::shiftOne(eastWest, types::Direction::SOUTHEAST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                eastWest = helper::shiftOne(index, types::Direction::WEST);
                northSouth = helper::shiftOne(index, types::Direction::SOUTH);
                diag = helper::shiftOne(northSouth, types::Direction::SOUTHEAST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = helper::shiftOne(eastWest, types::Direction::SOUTHWEST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = helper::shiftOne(northSouth, types::Direction::SOUTHWEST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                diag = helper::shiftOne(eastWest, types::Direction::NORTHWEST);
                if (existsCapturable(index, diag)) {
                    out.emplace_back(diag);
                }
                return out;
            }

            // returns if the piece at i is a blocker
            bool isBlocker(types::boardIndex i) {
                return boardVector[i].type() != types::PieceType::EMPTY;
            }
            // returns the list of on board ray indices in a given direction from a given src
            std::vector<types::boardIndex> getRay(types::boardIndex src, types::Direction d) {
                // src = 193
                // d = NORTH
                std::vector<types::boardIndex> out {};
                types::boardIndex next = helper::shiftOne(src, d);
                // next = 209
                while (boardVector[next].type() == types::PieceType::EMPTY) {
                    out.emplace_back(next);
                    next = helper::shiftOne(next, d);
                }
                // found blocker
                // add if capturable (blocker can be an out of bounds block piece)
                if (existsCapturable(src, next)) {
                    out.emplace_back(next);
                }
                return out;
            }
            std::vector<types::boardIndex> bishopShift(types::boardIndex src) {
                std::vector<types::boardIndex> out = getRay(src, types::Direction::NORTHEAST);
                helper::concat(out, getRay(src, types::Direction::NORTHWEST));
                helper::concat(out, getRay(src, types::Direction::SOUTHEAST));
                helper::concat(out, getRay(src, types::Direction::SOUTHWEST));
                return out;
            }

            std::vector<types::boardIndex> rookShift(types::boardIndex src) {
                std::vector<types::boardIndex> out = getRay(src, types::Direction::NORTH);
                helper::concat(out, getRay(src, types::Direction::EAST));
                helper::concat(out, getRay(src, types::Direction::SOUTH));
                helper::concat(out, getRay(src, types::Direction::WEST));
                return out;
            }

            std::vector<types::boardIndex> queenShift(types::boardIndex src) {
                std::vector<types::boardIndex> out = rookShift(src);
                helper::concat(out, bishopShift(src));
                return out;
            } 

            std::vector<types::boardIndex> kingShift(types::boardIndex src) {
                std::vector<types::Direction> directions = {types::Direction::NORTH, types::Direction::NORTHEAST, types::Direction::EAST, types::Direction::SOUTHEAST, types::Direction::SOUTH, types::Direction::SOUTHWEST, types::Direction::WEST, types::Direction::NORTHWEST};
                std::vector<types::boardIndex> out;
                types::boardIndex tmp;
                for (types::Direction d : directions) {
                    tmp = helper::shiftOne(src, d);
                    if (existsCapturable(src, tmp)) {
                        out.emplace_back(tmp);
                    }
                }

                // calculate castling possibility
                if (hasMoved(src)) {
                    return out;
                }
                auto leftRay = getRay(src, helper::getLeft(boardVector[src].colour()));
                auto rightRay = getRay(src, helper::getRight(boardVector[src].colour())); 

                if (leftRay.size() > 0) {
                    auto endLeftIndex = leftRay.back();
                    if (boardVector[endLeftIndex].type() == types::PieceType::ROOK && hasMoved(endLeftIndex) == false) {
                        out.emplace_back(endLeftIndex);
                    }
                }
                if (rightRay.size() > 0) {
                    auto endRightIndex = rightRay.back();
                    if (boardVector[endRightIndex].type() == types::PieceType::ROOK && hasMoved(endRightIndex) == false) {
                        out.emplace_back(endRightIndex);
                    }
                }
                return out;
            }

            // indices passed into this should be verified that no other piece blocks the path between them
            bool isCastling(types::boardIndex src, types::boardIndex tgt) {
                types::Square s = boardVector[src];
                types::Square t = boardVector[tgt];
                if (!(s.type() == types::PieceType::KING && t.type() == types:: PieceType::ROOK && s.colour() == t.colour())) {
                    return false;
                }

                if (hasMoved(src) || hasMoved(tgt)) {
                    return false;
                }
                return true;
            }

            

            // checks whether 2 indices constitute an enPeasant move
            bool isEnPeasant(types::boardIndex src, types::boardIndex trgt) {
                if (trgt != helper::shiftOne(src, helper::getLeft(boardVector[src].colour())) || 
                    trgt != helper::shiftOne(src, helper::getRight(boardVector[src].colour()))) {
                    return false;
                }
                if (boardVector[src].type() != types::PieceType::PAWN || boardVector[trgt].type() != types:: PieceType::PAWN) {
                    return false;
                }
                return true;
            }

            std::vector<types::Square> getBoard() {
                return boardVector;
            }
            
            int getValue(types::PieceType t) {
                switch (t) {
                    case types::PieceType::PAWN:
                        return 1;
                    case types::PieceType::ROOK:
                        return 5;
                    case types::PieceType::BISHOP:
                        return 3;
                    case types::PieceType::KNIGHT:
                        return 3;
                    case types::PieceType::QUEEN:
                        return 9;
                    case types::PieceType::KING:
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
                    boardVector.emplace_back(helper::generateSquare(i));
                }
                turn = helper::initailTurn;
                moveVector = std::vector<types::Move> {};
                playableColours = {true, true, true, true};
                players.push_back(std::ref(redPlayer));
                players.push_back(std::ref(bluePlayer));
                players.push_back(std::ref(yellowPlayer));
                players.push_back(std::ref(greenPlayer));
            }
            
            types::PieceColour getCurrentTurn() {
                return turn;
            }

            void setPlayerCheckmate(types::PieceColour c) {
                players[helper::indexFromColour(c)].get().setIsCheckmate(true);
            }
            // returns (c)'s last move if no move is found it returns the last move played
            types::Move & getLastMove(types::PieceColour c) {
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
                for (short i = helper::PADDEDROWS - 1; i >= 0; --i) {
                    for (int j = 0; j < helper::PADDEDCOLS; ++j) {
                        types::PieceType type = boardVector[16 * i + j].type();
                        std::cout << helper::PieceTypeToString(type).substr(0, 2) << " ";
                    }
                    std::cout << '\n';
                }
            }

            // asks whether c's king is in check in current board position
            bool isKingInCheck(types::PieceColour c) {
                assert(c != types::PieceColour::NONE);
                player::Player player = players[helper::indexFromColour(c)];
                // BUG this does not adequately check whether the king is in check
                // knight just fucking takes the blue king
                // idea
                // get king index
                std::set<types::boardIndex> s = player.getPieces()[helper::indexFromType(types::PieceType::KING)].get();
                auto node = s.extract(s.begin());
                assert(!node.empty());
                types::boardIndex kingIndex = node.value();

                // generate possible attacks

                // check ray attacks () and 
                std::vector<types::Direction> diags {types::Direction::NORTHEAST, types::Direction::SOUTHEAST, types::Direction::SOUTHWEST, types::Direction::NORTHWEST};
                std::vector<types::Direction> upDowns {types::Direction::NORTH, types::Direction::EAST, types::Direction::SOUTH, types::Direction::WEST};
                for (types::Direction d : diags) {
                    // sliding piece check
                    auto ray = getRay(kingIndex, d);
                    if (ray.size() > 0) {
                        auto last = ray.back();
                        if (existsEnemyPiece(kingIndex, last) && (boardVector.at(last).type() == types::PieceType::BISHOP || boardVector.at(last).type() == types:: PieceType::QUEEN)) {
                            return true;
                        }
                    }

                    // pawn check
                    auto diagIndex = helper::shiftOne(kingIndex, d);
                    if (existsEnemyPiece(kingIndex, diagIndex) && (boardVector.at(diagIndex).type() == types::PieceType::PAWN)) {
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

                for (types::Direction d : upDowns) {
                    auto ray = getRay(kingIndex, d);
                    if (ray.size() > 0) {
                        auto last = ray.back();
                        if (existsEnemyPiece(kingIndex, last) && (boardVector.at(last).type() == types::PieceType::ROOK || boardVector.at(last).type() == types:: PieceType::QUEEN)) {
                            return true;
                        }
                    }
                }

                // check knight
                auto knightRays = knightShift(kingIndex);
                assert(knightRays.size() < 9);
                for (types::boardIndex i : knightRays) {
                    if (existsEnemyPiece(kingIndex, i) && boardVector.at(i).type() == types::PieceType::KNIGHT) {
                        // std::cout << "knight ray found\n";
                        return true;
                    }
                }
                return false;
            }

            // generates the colours legal moves
            // returned moves can be guaranteed to be legal and all information except totalMoves to be correct
            std::vector<types::Move> generateLegalMoves(types::PieceColour c) {
                std::vector<types::Move> out;
                auto moves = generatePseudoLegalMoves(c);
                for (auto m : moves) {
                    virtualPlayMove(m);
                    if (!isKingInCheck(c)) {
                        out.emplace_back(m);
                    }
                    virtualUnPlayMove();
                }
                return out;
            }

            // gets the next colour such that the next colour's player is not checkmate
            // if there are no more players that aren't in checkmate it returns the provided colour
            // takes the colour to increment and the iteration counter
            // iteration should start on 0
            types::PieceColour getPrevTurn(types::PieceColour c, unsigned int iteration) {
                if (iteration == 4) {
                    return c;
                }
                switch (c) {
                    case types::PieceColour::BLUE:
                        return players[helper::indexFromColour(types::PieceColour::RED)].get().isCheckmate() ? getPrevTurn(types::PieceColour::RED, ++iteration) : types::PieceColour::RED;
                    case types::PieceColour::YELLOW:
                        return players[helper::indexFromColour(types::PieceColour::BLUE)].get().isCheckmate() ? getPrevTurn(types::PieceColour::BLUE, ++iteration) : types::PieceColour::BLUE;
                    case types::PieceColour::GREEN:
                        return players[helper::indexFromColour(types::PieceColour::YELLOW)].get().isCheckmate() ? getPrevTurn(types::PieceColour::YELLOW, ++iteration) : types::PieceColour::YELLOW;
                    case types::PieceColour::RED:
                        return players[helper::indexFromColour(types::PieceColour::GREEN)].get().isCheckmate() ? getPrevTurn(types::PieceColour::GREEN, ++iteration) : types::PieceColour::GREEN;
                    default:
                        return types::PieceColour::NONE;
                }
                return types::PieceColour::NONE;
            }

            // gets the next colour such that the next colour's player is not checkmate
            // if there are no more players that aren't in checkmate it returns the provided colour
            // takes the colour to increment and the iteration counter
            // iteration should start on 0
            types::PieceColour getNextTurn(types::PieceColour c, unsigned int iteration) {
                if (iteration == 4) {
                    return c;
                }
                switch (c) {
                    case types::PieceColour::RED:
                        return players[helper::indexFromColour(types::PieceColour::BLUE)].get().isCheckmate() ? getNextTurn(types::PieceColour::BLUE, ++iteration) : types::PieceColour::BLUE;
                    case types::PieceColour::BLUE:
                        return players[helper::indexFromColour(types::PieceColour::YELLOW)].get().isCheckmate() ? getNextTurn(types::PieceColour::YELLOW, ++iteration) : types::PieceColour::YELLOW;
                    case types::PieceColour::YELLOW:
                        return players[helper::indexFromColour(types::PieceColour::GREEN)].get().isCheckmate() ? getNextTurn(types::PieceColour::GREEN, ++iteration) : types::PieceColour::GREEN;
                    case types::PieceColour::GREEN:
                        return players[helper::indexFromColour(types::PieceColour::RED)].get().isCheckmate() ? getNextTurn(types::PieceColour::RED, ++iteration) : types::PieceColour::RED;
                    default:
                        return types::PieceColour::NONE;
                }
                return types::PieceColour::NONE;
            }
            
            // plays the move without checking whether the current turn is accurate to the move being played
            // should only be used when generating legal moves to see if the king is in check
            // this is because sometimes legal moves want to be created out of move order
            // also does not increment or decrement the turn clock
            void virtualPlayMove(types::Move m) {


                // update player data
                unsigned int pIndex = helper::indexFromColour(m.fromColour());
                players[pIndex].get().update(m);
                if (m.isCapture()) {
                    for (unsigned int i = 0; i < players.size(); ++i) {
                        if (pIndex != i) {
                            player::Player &p = players[i].get();
                            if (p.update(m) != types::PieceType::EMPTY) {
                                // after calling the above with m = 68, 83 the 83 from blue has not been removed
                                break;
                            }
                        }
                    }
                }
                
                // update board data
                // promotion
                if (m.isPromotion()) {
                    boardVector[m.toIndex()].setPiece(m.promotionPiece(), m.fromColour());
                    boardVector[m.fromIndex()].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                    
                    moveVector.emplace_back(m);
                    return;
                }

                if (m.isEnPeasant()) {
                    // NOTE enpeasant is encoded as src = pawn location and trgt = location of the adjascent pawn
                    types::boardIndex tmp = helper::shiftOne(m.toIndex(), helper::getUp(m.fromColour()));
                    boardVector[tmp].setPiece(m.fromPiece(), m.fromColour());
                    boardVector[m.fromIndex()].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                    boardVector[m.toIndex()].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
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
                    boardVector[m.fromIndex()].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                    boardVector[m.toIndex()].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);

                    
                    moveVector.emplace_back(m);
                    return;
                }

                // quiet moves
                assert (helper::indexOnBoard(m.fromIndex()) && helper::indexOnBoard(m.toIndex()));
                boardVector[m.toIndex()].setPiece(m.fromPiece(), m.fromColour());
                boardVector[m.fromIndex()].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                moveVector.emplace_back(m);

            }

            void playMove(types::Move m) {
                // runtime check that everything is working smoothly
                if ((boardVector[m.fromIndex()].type() != m.fromPiece()) || (boardVector[m.toIndex()].type() != m.capturedPiece())) {
                    printBoard();
                    std::string s = "types::Move is poorly formed: board square does not match move square data. Board type: " 
                    + std::string(helper::PieceTypeToString(boardVector[m.fromIndex()].type())) + "(" + std::to_string(m.fromIndex()) + ")" 
                    + ", " + std::string(helper::PieceTypeToString(m.fromPiece())) + "(" + std::to_string(m.fromIndex()) + ")" + "\n";
                    throw std::invalid_argument(s);
                }

                if (boardVector[m.fromIndex()].colour() != turn) {
                    std::string s = "It is not the given move's turn. Turn: " + std::string(helper::PieceColourToString(turn)) 
                        + " colour: " + std::string(helper::PieceColourToString(m.fromColour())) + " board piece colour: " 
                        + std::string(helper::PieceColourToString(boardVector[m.fromIndex()].colour())) + " move: "
                        + helper::moveToString(m) + "\n";
                    throw std::invalid_argument(s);
                }

                turn = getNextTurn(turn, 0);
                virtualPlayMove(m);
            }

            // virtually unplays the last move
            // unplays last move in the mvoestack
            // does not change the turn clock
            void virtualUnPlayMove() {
                
                // handle board data
                types::Move lastMove = moveVector.back(); //retreive last elem 
                // clear special move squares (Squares altered outside of src and trgt)
                if (lastMove.isCastling()) {
                    // clear everything inbetween
                    types::Direction d = helper::getDirection(lastMove.fromIndex(), lastMove.toIndex());
                    types::boardIndex tmp = lastMove.fromIndex();
                    while (tmp != lastMove.toIndex()) {
                        assert(isOnBoard(tmp));
                        boardVector[tmp].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                        tmp = helper::shiftOne(tmp, d);
                    }
                }
                if (lastMove.isEnPeasant()) {
                    boardVector[helper::shiftOne(lastMove.toIndex(), helper::getUp(lastMove.fromColour()))].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);                    
                }
                boardVector[lastMove.toIndex()].setPiece(lastMove.capturedPiece(), lastMove.capturedColour()); //return data to state
                boardVector[lastMove.fromIndex()].setPiece(lastMove.fromPiece(), lastMove.fromColour()); 
                moveVector.pop_back(); //remove from stack
                
                // handle player data
                // find the involved pieces last move 
                types::Move fromPrevMove; 
                types::Move toPrevMove; 
                for (types::Move m : moveVector) {
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

            // unplays the last played move
            void unPlayMove() {
                turn = getPrevTurn(turn, 0);// restd::set turn counter
                virtualUnPlayMove();
            }
    
            // gets the current material value of a colour 
            float getMaterial(types::PieceColour c) {
                float res = 0;
                for (auto square : boardVector) {
                    if (square.colour() == c) {
                        res += getValue(square.type());
                    }
                }
                return res;
            }

            // returns the position of the given colour
            std::vector<std::pair<types::Square, types::boardIndex>> getPosition(types::PieceColour c) {
                std::vector<std::pair<types::Square, types::boardIndex>> res;
                for (unsigned int i = 0; i < boardVector.size(); ++i) {
                    types::Square square = boardVector[i];
                    if (square.colour() == c) {
                        res.emplace_back(std::pair<types::Square, types::boardIndex> {square, i});
                    }
                }    
                return res;            
            }   

            bool movesPlayed() {
                return moveVector.size() != 0;
            }            
        
            types::Move indicesToMove(types::boardIndex from, types::boardIndex to) {
                types::PieceType promotionType = isPromotion(from, to) ? types::PieceType::QUEEN : types:: PieceType::EMPTY;
            
                bool special = isCastling(from, to) || isEnPeasant(from, to);
                return types::Move(from, to, 0, 
                boardVector[from].type(), boardVector[from].colour(),
                boardVector[to].type(), boardVector[to].colour(),
                promotionType, special);
            }
            
            // generates the appropriate shift of the given index
            std::vector<types::boardIndex> genShift(types::boardIndex from) {
                switch (boardVector[from].type()) {
                    case types::PieceType::PAWN:
                        return pawnShift(from);
                    case types::PieceType::ROOK:
                        return rookShift(from);
                    case types::PieceType::KNIGHT:
                        return knightShift(from);
                    case types::PieceType::BISHOP:
                        return bishopShift(from);
                    case types::PieceType::KING:
                        return kingShift(from);
                    case types::PieceType::QUEEN:
                        return queenShift(from);
                    default:
                        return {};
                }
            }

            bool isPlayerCheckmate(types::PieceColour c) {
                return players[helper::indexFromColour(c)].get().isCheckmate();
            }
    };

    

    
};

#endif