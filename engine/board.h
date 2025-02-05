#include"types.h"
#include"helper.h"
#include"playerData.h"

#include<vector>
#include<string>
#include<stack>
#include<list>
#include<memory>
#include<iostream>

#ifndef BOARD_H
#define BOARD_H

namespace board {
    // board is padded to 16x18
    class Board {
        std::array<types::Square, 288UL> boardArray; 
        
        types::PieceColour turn = helper::initailTurn;
        std::stack<types::Move, std::list<types::Move>> moveStack; 

        player::Player redPlayer = player::Player(types::PieceColour::RED);
        player::Player bluePlayer = player::Player(types::PieceColour::BLUE);
        player::Player yellowPlayer = player::Player(types::PieceColour::YELLOW);
        player::Player greenPlayer = player::Player(types::PieceColour::GREEN);

        const std::array<const std::reference_wrapper<player::Player>, 4UL> players {{std::ref(redPlayer), std::ref(bluePlayer), std::ref(yellowPlayer), std::ref(greenPlayer)}};

        private:

            inline std::stack<types::Move, std::list<types::Move>> getMoveHistory() const {
                return moveStack;
            }
            types::Square & getSquare(types::boardIndex r, types::boardIndex c) { // from 14x14
                const short a1 = 33;
                return boardArray.at(r * helper::PADDEDCOLS + (c + 1) + a1);
            }
            // types::Square & getSquareByType(COLUMN c, short r) {}
            types::Square & getSquarePadded(types::boardIndex r, types::boardIndex c) { // from 16x18
                return boardArray.at(r * 16 + c);
            }
            // helper function to query board if the piece at s can capture at t also checking if t exists (can be anything on board as long as diff colours)
            // note this is pseudo legal and hence should not be used as a definitive answer
            // assume src is a valid piece ie not empty
            // checks if target is on the board
            constexpr bool existsCapturable(types::boardIndex src, types::boardIndex trgt) const {

                types::Square srcSqu = boardArray.at(src);
                types::Square trgtSqu = boardArray.at(trgt);
                return isOnBoard(trgt) && trgtSqu.colour() != srcSqu.colour() && trgtSqu.isAccessible();
            }
            // STRICTER THAN existsCapturable
            constexpr bool existsEnemyPiece(types::boardIndex src, types::boardIndex trgt) const {
                types::Square trgtSqu = boardArray.at(trgt);
                types::Square srcSqu = boardArray.at(src);
                return isOnBoard(trgt) && trgtSqu.colour() != srcSqu.colour() && !trgtSqu.isEmpty() && trgtSqu.isAccessible();
            }            

            constexpr bool isPromotion(types::boardIndex from, types::boardIndex to) const {
                if (boardArray[from].type() != types::PieceType::PAWN) {
                    return false;
                }
                switch (boardArray[from].colour()) {
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
            void bulkCreateMove(types::boardIndex src, std::vector<types::boardIndex> trgts, std::vector<std::unique_ptr<types::Move>> &out) {
                assert(helper::indexOnBoard(src));
                out.reserve(trgts.size());
                for (types::boardIndex t : trgts) {
                    
                    if (boardArray[src].type() == types::PieceType::PAWN && isPromotion(src, t)) {
                        out.push_back(std::make_unique<types::Move>(
                            src, t, 0,
                            boardArray[src].type(), boardArray[src].colour(), 
                            boardArray[t].type(), boardArray[t].colour(),
                            types::PieceType::QUEEN
                        ));
                        continue;
                    } 
                    else if (/*isEnPeasant(src, t) ||*/ isCastling(src, t)){
                        out.push_back(std::make_unique<types::Move>(
                            src, t, 0,
                            boardArray[src].type(), boardArray[src].colour(), 
                            boardArray[t].type(), boardArray[t].colour(),
                            types::PieceType::EMPTY, true
                        ));
                        continue;
                    }
                    out.push_back(std::make_unique<types::Move>(
                        src, t, 0,
                        boardArray[src].type(), boardArray[src].colour(), 
                        boardArray[t].type(), boardArray[t].colour()));
                }
                return;
            }


            // check if a square is empty by index
            constexpr bool isEmpty(types::boardIndex i) const {
                return boardArray[i].type() == types::PieceType::EMPTY;
            }

            constexpr bool isOnBoard(types::boardIndex i) const{
                for (types::boardIndex c : helper::cornerIndices) { // not in corners
                    if (c == i) {
                        return false;
                    }
                }
                types::boardIndex rem = i % 16;
                return rem != 0 && rem != 15 && i < 252 && i > 35;
            }

            const bool hasMoved(types::boardIndex i) const {
                auto c = boardArray[i].colour();
                player::Player p = players[helper::indexFromColour(c)];
                return p.indexHasMoved(i);
            }
        
            // returns if the piece at i is a blocker
            constexpr bool isBlocker(types::boardIndex i) const {
                return boardArray[i].type() != types::PieceType::EMPTY;
            }

            // includes en peasant
            std::vector<types::boardIndex> pawnCaptureShift(types::boardIndex index) {
                // check if there exists a capturable piece beside 
                // TODO optimise later
                std::vector<types::boardIndex> out = std::vector<types::boardIndex> {}; // en peasant is unlikely 
                // regular captures
                types::boardIndex target = helper::shiftOne(index, helper::getUpRight(boardArray[index].colour()));
                // bool upRightEmpty = isEmpty(target);
                if (existsEnemyPiece(index, target)) {
                    out.emplace_back(target);
                }

                target = helper::shiftOne(index, helper::getUpLeft(boardArray[index].colour())); // go to next possible capture
                // bool upLeftEmpty = isEmpty(target);
                if (existsEnemyPiece(index, target)) {
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

            // returns the list of on board ray indices in a given direction from a given src
            std::vector<types::boardIndex> getRay(types::boardIndex src, types::Direction d) {
                // src = 193
                // d = NORTH
                std::vector<types::boardIndex> out {};
                types::boardIndex next = helper::shiftOne(src, d);
                // next = 209
                while (boardArray[next].type() == types::PieceType::EMPTY) {
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
                auto leftRay = getRay(src, helper::getLeft(boardArray[src].colour()));
                auto rightRay = getRay(src, helper::getRight(boardArray[src].colour())); 

                if (leftRay.size() > 0) {
                    auto endLeftIndex = leftRay.back();
                    if (boardArray[endLeftIndex].type() == types::PieceType::ROOK && hasMoved(endLeftIndex) == false) {
                        out.emplace_back(endLeftIndex);
                    }
                }
                if (rightRay.size() > 0) {
                    auto endRightIndex = rightRay.back();
                    if (boardArray[endRightIndex].type() == types::PieceType::ROOK && hasMoved(endRightIndex) == false) {
                        out.emplace_back(endRightIndex);
                    }
                }
                return out;
            }

            // indices passed into this should be verified that no other piece blocks the path between them
            bool isCastling(types::boardIndex src, types::boardIndex tgt) {
                types::Square s = boardArray[src];
                types::Square t = boardArray[tgt];
                if (!(s.type() == types::PieceType::KING && t.type() == types:: PieceType::ROOK && s.colour() == t.colour())) {
                    return false;
                }

                if (hasMoved(src) || hasMoved(tgt)) {
                    return false;
                }
                return true;
            }


            std::array<types::Square, 288UL> getBoard() const {
                return boardArray;
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


            // Quiet move shift checks if on the board
            std::vector<types::boardIndex> pawnQuietShift(types::boardIndex index) {
                std::vector<types::boardIndex> out = {};
                types::boardIndex m = helper::shiftOne(index, helper::getUp(turn));
                if (isEmpty(m) && isOnBoard(m)) {out.emplace_back(m);}
                if (!helper::isPawnStart(index)) {
                    return out;
                }
                m = helper::shiftOne(m, helper::getUp(turn));
                if (isEmpty(m) && isOnBoard(m)) {
                    out.emplace_back(m);
                }
                return out;
            }
            
            std::vector<types::boardIndex> pawnShift(types::boardIndex index) {
                std::vector<types::boardIndex> out = pawnQuietShift(index);
                helper::concat(out, pawnCaptureShift(index));
                return out;
            }

            void pawnQuietShift1(types::boardIndex index, std::vector<types::boardIndex> &out) {
                types::boardIndex m = helper::shiftOne(index, helper::getUp(turn));
                if (isEmpty(m) && isOnBoard(m)) {out.emplace_back(m);}
                if (!helper::isPawnStart(index)) {
                    return;
                }
                m = helper::shiftOne(m, helper::getUp(turn));
                if (isEmpty(m) && isOnBoard(m)) {
                    out.emplace_back(m);
                }
            }

            void pawnShift4(types::boardIndex index, std::vector<types::boardIndex> &out) {
                out.reserve(4);
                pawnQuietShift1(index, out);
                std::vector<types::boardIndex> tmp = pawnCaptureShift(index);
                for (types::boardIndex i : tmp) {
                    out.emplace_back(i);
                }
            }

            void pawnQuietShift2(types::boardIndex index, std::array<types::boardIndex, 4> &out) {
                types::boardIndex m = helper::shiftOne(index, helper::getUp(turn));
                if (isEmpty(m) && isOnBoard(m)) {
                    out[0] = m;
                }
                if (helper::isPawnStart(index)) {
                    m = helper::shiftOne(m, helper::getUp(turn));
                    if (isEmpty(m) && isOnBoard(m)) {
                        out[1] = m;
                    }
                }
            }


            // includes en peasant
            void pawnCaptureShift1(types::boardIndex index, std::array<types::boardIndex, 4> &out) {
                // check if there exists a capturable piece beside 
                // regular captures
                types::boardIndex target = helper::shiftOne(index, helper::getUpRight(boardArray[index].colour()));
                // bool upRightEmpty = isEmpty(target);
                if (existsEnemyPiece(index, target)) {
                    out[2] = target;
                }

                target = helper::shiftOne(index, helper::getUpLeft(boardArray[index].colour())); // go to next possible capture
                // bool upLeftEmpty = isEmpty(target);
                if (existsEnemyPiece(index, target)) {
                    out[3] = target;
                }
            }

            void pawnShift5(types::boardIndex index, std::array<types::boardIndex, 4> &out) {
                out = {300UL, 300UL, 300UL, 300UL};
                pawnQuietShift2(index, out);
                pawnCaptureShift1(index, out);
            }

            // generates pseudo legal moves
            // moves in returned std::vector have all except the totalMoves guaranteed to be filled
            void generatePseudoLegalMoves(types::PieceColour c, std::vector<std::unique_ptr<types::Move>> &out) {
                player::Player &player = players[helper::indexFromColour(c)].get();
                std::array<const std::reference_wrapper<std::set<types::boardIndex>>, 6> pieces = player.getPieces();
                out.reserve(111);
                for (unsigned int i = 0; i < pieces.size(); ++i) {
                    std::set<types::boardIndex> &s = pieces[i].get();
                    for (auto index : s) {
                        switch (helper::playablePieces[i]) {
                            case types::PieceType::PAWN:
                            bulkCreateMove(index, pawnShift(index), out);
                            break;
                        case types::PieceType::ROOK:
                            bulkCreateMove(index, rookShift(index), out);
                            break;
                        case types::PieceType::KNIGHT:
                            bulkCreateMove(index, knightShift(index), out);
                            break;
                        case types::PieceType::BISHOP:
                            bulkCreateMove(index, bishopShift(index), out);
                            break;
                        case types::PieceType::KING:
                            bulkCreateMove(index, kingShift(index), out);
                            break;
                        case types::PieceType::QUEEN:
                            bulkCreateMove(index, queenShift(index), out);
                            break;
                        default:
                            continue;
                        }
                    }
                }
                out.shrink_to_fit();
            }
            Board() {
                // construct to initial position
                for (short i = 0; i < 288; ++i) {
                    boardArray[i] = helper::generateSquare(i);
                }
                // players[0] = std::ref(redPlayer);
                // players[1] = std::ref(bluePlayer);
                // players[2] = std::ref(yellowPlayer);
                // players[3] = std::ref(greenPlayer);
            }

            Board(Board &b) :
            boardArray(b.boardArray), 
            turn(b.turn),
            moveStack(b.moveStack),
            redPlayer(b.redPlayer),
            bluePlayer(b.bluePlayer),
            yellowPlayer(b.yellowPlayer),
            greenPlayer(b.greenPlayer),
            players({ std::ref(redPlayer), std::ref(bluePlayer), std::ref(yellowPlayer), std::ref(greenPlayer) })
            {
            }
            Board& operator=(Board& other) = default;
            constexpr std::array<const std::reference_wrapper<player::Player>, 4UL>  getPlayers() const {
                return players;
            }


            constexpr types::PieceColour getCurrentTurn() const {
                return turn;
            }

            void setPlayerCheckmate(types::PieceColour c) {
                players[helper::indexFromColour(c)].get().setIsCheckmate(true);
            }

            // // returns (c)'s last move if no move is found it returns the last move played
            // types::Move & getLastMove(types::PieceColour c) {
            //     assert(moveStack.size() > 0);
            //     std::stack<types::Move, std::list<types::Move>> tmp;
            //     unsigned int i = 0;
            //     while(moveStack.empty() == false && i < 4) {
            //         if (moveStack.top().fromColour() == c) {
            //             types::Move &ret = moveStack.top();
            //             while (!tmp.empty()) {
            //                 moveStack.push(tmp.top());
            //                 tmp.pop();
            //             }
            //             return ret;
            //         }
            //         ++i;
            //     }
            //     while (!tmp.empty()) {
            //         moveStack.push(tmp.top());
            //         tmp.pop();
            //     }
            //     return moveStack.top();
            // } 

            void printPaddedBoard() {
                // algo is weird but prints correctly according to red
                // orientation in order NESW = YGRB
                for (short i = helper::PADDEDROWS - 1; i >= 0; --i) {
                    for (int j = 0; j < helper::PADDEDCOLS; ++j) {
                        types::PieceType type = boardArray[16 * i + j].type();
                        std::cout << helper::typeToChar(type) << " ";
                    }
                    std::cout << '\n';
                }
            }

            void printBoard() {
                for (short i = helper::PADDEDROWS - 3; i >= 2; --i) {
                    for (int j = 1; j < helper::PADDEDCOLS - 1; ++j) {
                        char colour = helper::colourToChar(boardArray[16 * i + j].colour());
                        char type = helper::typeToChar(boardArray[16 * i + j].type());
                        if (colour != ' ') {
                            std::cout << colour << type << " ";
                        } else {
                            std::cout << type << type << " ";
                        }

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
                        if (existsEnemyPiece(kingIndex, last) && (boardArray.at(last).type() == types::PieceType::BISHOP || boardArray.at(last).type() == types:: PieceType::QUEEN)) {
                            return true;
                        }
                    }

                    // pawn check
                    auto diagIndex = helper::shiftOne(kingIndex, d);
                    if (existsEnemyPiece(kingIndex, diagIndex) && (boardArray.at(diagIndex).type() == types::PieceType::PAWN)) {
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
                        if (existsEnemyPiece(kingIndex, last) && (boardArray.at(last).type() == types::PieceType::ROOK || boardArray.at(last).type() == types:: PieceType::QUEEN)) {
                            return true;
                        }
                    }
                }

                // check knight
                auto knightRays = knightShift(kingIndex);
                assert(knightRays.size() < 9);
                for (types::boardIndex i : knightRays) {
                    if (existsEnemyPiece(kingIndex, i) && boardArray.at(i).type() == types::PieceType::KNIGHT) {
                        // std::cout << "knight ray found\n";
                        return true;
                    }
                }
                return false;
            }

            // generates the colours legal moves
            // returned moves can be guaranteed to be legal and all information except totalMoves to be correct
            void generateLegalMoves(types::PieceColour c, std::vector<std::unique_ptr<types::Move>> &out) {
                std::vector<std::unique_ptr<types::Move>> tmp;
                generatePseudoLegalMoves(c, tmp);
                out.reserve(tmp.size());
                for (std::unique_ptr<types::Move> &m : tmp) {
                    virtualPlayMove(*m);
                    if (!isKingInCheck(c)) {
                        out.push_back(std::move(m));
                    }
                    virtualUnPlayMove();
                }
                out.shrink_to_fit();
                return;
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
                    boardArray[m.toIndex()].setPiece(m.promotionPiece(), m.fromColour());
                    boardArray[m.fromIndex()].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                    
                    moveStack.push(m);
                    return;
                }

                // if (m.isEnPeasant()) {
                //     // NOTE enpeasant is encoded as src = pawn location and trgt = location of the adjascent pawn
                //     types::boardIndex tmp = helper::shiftOne(m.toIndex(), helper::getUp(m.fromColour()));
                //     boardArray[tmp].setPiece(m.fromPiece(), m.fromColour());
                //     boardArray[m.fromIndex()].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                //     boardArray[m.toIndex()].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                //     moveStack.push(m);
                //     return;
                // }


                if (m.isCastling()) {
                    int dif = m.toIndex() - m.fromIndex();
                    int absDif = abs(dif);
                    if (absDif == 3 || absDif == 48) {
                        // queenside castle
                        boardArray[m.fromIndex() + (dif / 3 * 2)].setPiece(m.fromPiece(), m.fromColour());
                    } else {
                        // kingside castle
                        boardArray[m.fromIndex() + dif].setPiece(m.fromPiece(), m.fromColour());
                    }
                    boardArray[m.toIndex() - dif].setPiece(m.capturedPiece(), m.capturedColour());
                    boardArray[m.fromIndex()].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                    boardArray[m.toIndex()].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);

                    
                    moveStack.push(m);
                    return;
                }

                // quiet moves
                assert (helper::indexOnBoard(m.fromIndex()) && helper::indexOnBoard(m.toIndex()));
                boardArray[m.toIndex()].setPiece(m.fromPiece(), m.fromColour());
                boardArray[m.fromIndex()].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                moveStack.push(m);

            }

            void playMove(types::Move m) {
                // runtime check that everything is working smoothly
                // std::cout << "nuts=" << helper::typeToChar(boardArray[m.fromIndex()].type()) << std::endl;
                // std::cout << "nuts=" << helper::typeToChar(m.fromPiece()) << std::endl;
                assert(boardArray[m.fromIndex()].type() == m.fromPiece());
                assert(boardArray[m.toIndex()].type() == m.capturedPiece());
                // if ((boardArray[m.fromIndex()].type() != m.fromPiece()) || (boardArray[m.toIndex()].type() != m.capturedPiece())) {
                //     printPaddedBoard();
                //     std::string s = "types::Move is poorly formed: board square does not match move square data. Board type: " 
                //     + helper::typeToChar(boardArray[m.fromIndex()].type()) + '(' + std::to_string(m.fromIndex()) + ")" 
                //     + ", " + helper::typeToChar(m.fromPiece()) + "(" + std::to_string(m.fromIndex()) + ")" + "\n";
                //     throw std::invalid_argument(s);
                // }

                assert(boardArray[m.fromIndex()].colour() == turn);
                // if (boardArray[m.fromIndex()].colour() != turn) {
                //     std::string s = "It is not the given move's turn. Turn: " + helper::colourToChar(turn) 
                //         + 'c' + ':' + ' ' + helper::colourToChar(m.fromColour()) + 'c' +  ':' + ' ' 
                //         + helper::colourToChar(boardArray[m.fromIndex()].colour()) + '\n';
                //     throw std::invalid_argument(s);
                // }

                turn = getNextTurn(turn, 0);
                virtualPlayMove(m);
            }

            // virtually unplays the last move
            // unplays last move in the mvoestack
            // does not change the turn clock
            void virtualUnPlayMove() {
                
                // handle board data
                types::Move lastMove = moveStack.top(); //retreive last elem 
                // clear special move squares (Squares altered outside of src and trgt)
                if (lastMove.isCastling()) {
                    // clear everything inbetween
                    types::Direction d = helper::getDirection(lastMove.fromIndex(), lastMove.toIndex());
                    types::boardIndex tmp = lastMove.fromIndex();
                    while (tmp != lastMove.toIndex()) {
                        assert(isOnBoard(tmp));
                        boardArray[tmp].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                        tmp = helper::shiftOne(tmp, d);
                    }
                }
                // if (lastMove.isEnPeasant()) {
                //     boardArray[helper::shiftOne(lastMove.toIndex(), helper::getUp(lastMove.fromColour()))].setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);                    
                // }
                boardArray[lastMove.toIndex()].setPiece(lastMove.capturedPiece(), lastMove.capturedColour()); //return data to state
                boardArray[lastMove.fromIndex()].setPiece(lastMove.fromPiece(), lastMove.fromColour()); 
                moveStack.pop(); //remove from stack
                
                // handle player data
                // find the involved pieces last move 
                types::Move fromPrevMove; 
                types::Move toPrevMove; 
                std::stack<types::Move, std::list<types::Move>> tmp;
                // std::cout << "clearing movestack\n";
                while (!moveStack.empty() && (fromPrevMove.fromIndex() == 300 || toPrevMove.fromIndex() == 300)) {
                    if (fromPrevMove.fromIndex() == 300 && moveStack.top().toIndex() == lastMove.fromIndex()) {
                        fromPrevMove = moveStack.top();
                    }
                    if (toPrevMove.fromIndex() == 300 && moveStack.top().toIndex() == lastMove.toIndex()) {
                        toPrevMove = moveStack.top();
                    }
                    tmp.push(moveStack.top());
                    moveStack.pop();
                }
                // std::cout << "movestack cleared\n";
                while (!tmp.empty()) {
                    moveStack.push(tmp.top());
                    tmp.pop();
                }
                // std::cout << "movestack reinstantiated\n";
                // for (types::Move m : moveStack) {
                //     if (m.toIndex() == lastMove.fromIndex()) {
                //         fromPrevMove = m;
                //     }
                //     if (m.toIndex() == lastMove.toIndex()) {
                //         toPrevMove = m;
                //     }
                // }

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
                for (auto square : boardArray) {
                    if (square.colour() == c) {
                        res += getValue(square.type());
                    }
                }
                return res;
            }

            // returns the position of the given colour
            std::vector<std::pair<types::Square, types::boardIndex>> getPosition(types::PieceColour c) {
                std::vector<std::pair<types::Square, types::boardIndex>> res;
                for (unsigned int i = 0; i < boardArray.size(); ++i) {
                    types::Square square = boardArray[i];
                    if (square.colour() == c) {
                        res.emplace_back(std::pair<types::Square, types::boardIndex> {square, i});
                    }
                }    
                return res;            
            }   

            bool movesPlayed() {
                return moveStack.size() != 0;
            }            
        
            types::Move indicesToMove(types::boardIndex from, types::boardIndex to) {
                types::PieceType promotionType = isPromotion(from, to) ? types::PieceType::QUEEN : types:: PieceType::EMPTY;
            
                bool special = isCastling(from, to) /*|| isEnPeasant(from, to)*/;
                return types::Move(from, to, 0, 
                boardArray[from].type(), boardArray[from].colour(),
                boardArray[to].type(), boardArray[to].colour(),
                promotionType, special);
            }
            
            // generates the appropriate shift of the given index
            std::vector<types::boardIndex> genShift(types::boardIndex from) {
                switch (boardArray[from].type()) {
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

            constexpr bool isPlayerCheckmate(types::PieceColour c) {
                return players[helper::indexFromColour(c)].get().isCheckmate();
            }
    };

    

    
};

#endif