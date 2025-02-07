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

// TODO convert the vector usage to array usage
// TOOD make a fixed size variably filled array
// requires a looping mechanism, 
// a size checker (how much of the array is correctly filled)
// a default incorrect value
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
                if (boardArray.at(from).type() != types::PieceType::PAWN) {
                    return false;
                }
                switch (boardArray.at(from).colour()) {
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
            constexpr void bulkCreateMove(types::boardIndex src, std::array<types::boardIndex, 41> trgts, std::vector<std::unique_ptr<types::Move>> &out) const {
                assert(helper::indexOnBoard(src));
                for (unsigned int i = 0; i < trgts.size() && trgts.at(i) != 0; ++i) {
                    types::boardIndex t = trgts.at(i);
                    if (boardArray.at(src).type() == types::PieceType::PAWN && isPromotion(src, t)) {
                        out.push_back(std::make_unique<types::Move>(
                            src, t, 0,
                            boardArray.at(src).type(), boardArray.at(src).colour(), 
                            boardArray.at(t).type(), boardArray.at(t).colour(),
                            types::PieceType::QUEEN
                        ));
                    } 
                    else if (/*isEnPeasant(src, t) ||*/ isCastling(src, t)){
                        out.push_back(std::make_unique<types::Move>(
                            src, t, 0,
                            boardArray.at(src).type(), boardArray.at(src).colour(), 
                            boardArray.at(t).type(), boardArray.at(t).colour(),
                            types::PieceType::EMPTY, true
                        ));
                    } else {
                        out.push_back(std::make_unique<types::Move>(
                            src, t, 0,
                            boardArray.at(src).type(), boardArray.at(src).colour(), 
                            boardArray.at(t).type(), boardArray.at(t).colour()));
                    }
                }
                return;
            }


            // check if a square is empty by index
            constexpr bool isEmpty(types::boardIndex i) const {
                return boardArray.at(i).type() == types::PieceType::EMPTY;
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
                auto c = boardArray.at(i).colour();
                player::Player p = players.at(helper::indexFromColour(c));
                return p.indexHasMoved(i);
            }
        
            // returns if the piece at i is a blocker
            constexpr bool isBlocker(types::boardIndex i) const {
                return boardArray.at(i).type() != types::PieceType::EMPTY;
            }

            // includes en peasant
            constexpr std::array<types::boardIndex, 2> pawnCaptureShift(types::boardIndex index) const {
                std::array<types::boardIndex, 2> out  = {};
                // regular captures
                types::boardIndex target = helper::shiftOne(index, helper::getUpRight(boardArray.at(index).colour()));
                // bool upRightEmpty = isEmpty(target);
                if (existsEnemyPiece(index, target)) {
                    out.at(0) = target;
                }

                target = helper::shiftOne(index, helper::getUpLeft(boardArray.at(index).colour())); // go to next possible capture
                // bool upLeftEmpty = isEmpty(target);
                if (existsEnemyPiece(index, target)) {
                    out.at(1) = target;
                }
                return out;
            }

            constexpr std::array<types::boardIndex, 41> knightShift(types::boardIndex index) const {
                std::array<types::boardIndex, 41> out = {}; 
                unsigned int i = 0;
                types::boardIndex northSouth = helper::shiftOne(index, types::Direction::NORTH);
                types::boardIndex eastWest = helper::shiftOne(index, types::Direction::EAST);
                types::boardIndex diag = helper::shiftOne(northSouth, types::Direction::NORTHEAST);
                if (existsCapturable(index, diag)) {
                    out.at(i++) = diag;
                }
                diag = helper::shiftOne(eastWest, types::Direction::NORTHEAST);
                if (existsCapturable(index, diag)) {
                    out.at(i++) = diag;
                }
                diag = helper::shiftOne(northSouth, types::Direction::NORTHWEST);
                if (existsCapturable(index, diag)) {
                    out.at(i++) = diag;
                }
                diag = helper::shiftOne(eastWest, types::Direction::SOUTHEAST);
                if (existsCapturable(index, diag)) {
                    out.at(i++) = diag;
                }
                eastWest = helper::shiftOne(index, types::Direction::WEST);
                northSouth = helper::shiftOne(index, types::Direction::SOUTH);
                diag = helper::shiftOne(northSouth, types::Direction::SOUTHEAST);
                if (existsCapturable(index, diag)) {
                    out.at(i++) = diag;
                }
                diag = helper::shiftOne(eastWest, types::Direction::SOUTHWEST);
                if (existsCapturable(index, diag)) {
                    out.at(i++) = diag;
                }
                diag = helper::shiftOne(northSouth, types::Direction::SOUTHWEST);
                if (existsCapturable(index, diag)) {
                    out.at(i++) = diag;
                }
                diag = helper::shiftOne(eastWest, types::Direction::NORTHWEST);
                if (existsCapturable(index, diag)) {
                    out.at(i++) = diag;
                }
                return out;
            }

            // returns the list of on board ray indices in a given direction from a given src
            constexpr std::array<types::boardIndex, 41> getRay(types::boardIndex src, types::Direction d) const{
                std::array<types::boardIndex, 41> out = {};
                unsigned int i = 0;
                types::boardIndex next = helper::shiftOne(src, d);
                // next = 209
                while (boardArray.at(next).type() == types::PieceType::EMPTY) {
                    out.at(i++) = next; 
                    next = helper::shiftOne(next, d);
                }
                // found blocker
                // add if capturable (blocker can be an out of bounds block piece)
                if (existsCapturable(src, next)) {
                    out.at(i++) = next; 
                }
                return out;
            }
            constexpr std::array<types::boardIndex, 41> bishopShift(types::boardIndex src) const {
                std::array<types::boardIndex, 41> out = getRay(src, types::Direction::NORTHEAST);
                helper::concat(out, getRay(src, types::Direction::NORTHWEST));
                helper::concat(out, getRay(src, types::Direction::SOUTHEAST));
                helper::concat(out, getRay(src, types::Direction::SOUTHWEST));
                return out;
            }

            constexpr std::array<types::boardIndex, 41> rookShift(types::boardIndex src) const {
                std::array<types::boardIndex, 41> out = getRay(src, types::Direction::NORTH);
                helper::concat(out, getRay(src, types::Direction::EAST));
                helper::concat(out, getRay(src, types::Direction::SOUTH));
                helper::concat(out, getRay(src, types::Direction::WEST));
                return out;
            }

            constexpr std::array<types::boardIndex, 41> queenShift(types::boardIndex src) const {
                std::array<types::boardIndex, 41> out = rookShift(src);
                helper::concat(out, bishopShift(src));
                return out;
            } 

            std::array<types::boardIndex, 41> kingShift(types::boardIndex src) const {
                std::array<types::boardIndex, 41> out = {};
                unsigned int i = 0;
                types::boardIndex tmp;
                for (types::Direction d : helper::DIRECTIONS) {
                    tmp = helper::shiftOne(src, d);
                    if (existsCapturable(src, tmp)) {
                        out.at(i++) = tmp;
                    }
                }

                // calculate castling possibility
                if (hasMoved(src)) {
                    return out;
                }
                auto leftRay = getRay(src, helper::getLeft(boardArray.at(src).colour()));
                auto rightRay = getRay(src, helper::getRight(boardArray.at(src).colour())); 

                if (leftRay.size() > 0) {
                    auto endLeftIndex = leftRay.back();
                    if (boardArray.at(endLeftIndex).type() == types::PieceType::ROOK && hasMoved(endLeftIndex) == false) {
                        out.at(i++) = endLeftIndex;
                    }
                }
                if (rightRay.size() > 0) {
                    auto endRightIndex = rightRay.back();
                    if (boardArray.at(endRightIndex).type() == types::PieceType::ROOK && hasMoved(endRightIndex) == false) {
                        out.at(i++) = endRightIndex;
                    }
                }
                return out;
            }

            // indices passed into this should be verified that no other piece blocks the path between them
            constexpr bool isCastling(types::boardIndex src, types::boardIndex tgt) const {
                types::Square s = boardArray.at(src);
                types::Square t = boardArray.at(tgt);
                if (!(s.type() == types::PieceType::KING && t.type() == types:: PieceType::ROOK && s.colour() == t.colour())) {
                    return false;
                }

                if (hasMoved(src) || hasMoved(tgt)) {
                    return false;
                }
                return true;
            }


            constexpr std::array<types::Square, 288UL> getBoard() const {
                return boardArray;
            }
        public:


            // Quiet move shift checks if on the board
            constexpr std::array<types::boardIndex, 41> pawnQuietShift(types::boardIndex index) const {
                std::array<types::boardIndex, 41> out = {};
                unsigned int i = 0;
                types::boardIndex m = helper::shiftOne(index, helper::getUp(turn));
                if (isEmpty(m) && isOnBoard(m)) {
                    out.at(i++) = m;
                }
                if (!helper::isPawnStart(index)) {
                    return out;
                }
                m = helper::shiftOne(m, helper::getUp(turn));
                if (isEmpty(m) && isOnBoard(m)) {
                    out.at(i++) = m;
                }
                return out;
            }
            
            constexpr std::array<types::boardIndex, 41> pawnShift(types::boardIndex index) const {
                std::array<types::boardIndex, 41> out = pawnQuietShift(index);
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

            // void pawnShift4(types::boardIndex index, std::vector<types::boardIndex> &out) {
            //     out.reserve(4);
            //     pawnQuietShift1(index, out);
            //     types::FLSArray tmp = pawnCaptureShift(index);
            //     for (unsigned int i = 0; i < tmp.size(); ++i) {
            //         types::boardIndex index = tmp.at(i);
            //         out.emplace_back(index);
            //     }
            // }

            void pawnQuietShift2(types::boardIndex index, std::array<types::boardIndex, 4> &out) {
                types::boardIndex m = helper::shiftOne(index, helper::getUp(turn));
                if (isEmpty(m) && isOnBoard(m)) {
                    out.at(0) = m;
                }
                if (helper::isPawnStart(index)) {
                    m = helper::shiftOne(m, helper::getUp(turn));
                    if (isEmpty(m) && isOnBoard(m)) {
                        out.at(1) = m;
                    }
                }
            }


            // includes en peasant
            void pawnCaptureShift1(types::boardIndex index, std::array<types::boardIndex, 4> &out) {
                // check if there exists a capturable piece beside 
                // regular captures
                types::boardIndex target = helper::shiftOne(index, helper::getUpRight(boardArray.at(index).colour()));
                // bool upRightEmpty = isEmpty(target);
                if (existsEnemyPiece(index, target)) {
                    out.at(2) = target;
                }

                target = helper::shiftOne(index, helper::getUpLeft(boardArray.at(index).colour())); // go to next possible capture
                // bool upLeftEmpty = isEmpty(target);
                if (existsEnemyPiece(index, target)) {
                    out.at(3) = target;
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
                player::Player &player = players.at(helper::indexFromColour(c)).get();
                std::array<const std::reference_wrapper<std::set<types::boardIndex>>, 6> pieces = player.getPieces();
                out.reserve(111);
                for (unsigned int i = 0; i < pieces.size(); ++i) {
                    std::set<types::boardIndex> &s = pieces.at(i).get();
                    // causes segfault when dereferencing this
                    std::for_each(s.begin(), s.end(), [&i, *this, &out](types::boardIndex index) {
                        switch (helper::playablePieces.at(i)) {
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
                            assert(false);
                        }
                    });
                }
                out.shrink_to_fit();
            }
            Board() {
                // construct to initial position
                for (short i = 0; i < 288; ++i) {
                    boardArray.at(i) = helper::generateSquare(i);
                }
                // players.at(0) = std::ref(redPlayer);
                // players.at(1) = std::ref(bluePlayer);
                // players.at(2) = std::ref(yellowPlayer);
                // players.at(3) = std::ref(greenPlayer);
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
                players.at(helper::indexFromColour(c)).get().setIsCheckmate(true);
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

            constexpr void printPaddedBoard() const {
                // algo is weird but prints correctly according to red
                // orientation in order NESW = YGRB
                for (short i = helper::PADDEDROWS - 1; i >= 0; --i) {
                    for (int j = 0; j < helper::PADDEDCOLS; ++j) {
                        types::PieceType type = boardArray.at(16 * i + j).type();
                        std::cout << helper::typeToChar(type) << " ";
                    }
                    std::cout << '\n';
                }
            }

            constexpr void printBoard() const {
                for (short i = helper::PADDEDROWS - 3; i >= 2; --i) {
                    for (int j = 1; j < helper::PADDEDCOLS - 1; ++j) {
                        char colour = helper::colourToChar(boardArray.at(16 * i + j).colour());
                        char type = helper::typeToChar(boardArray.at(16 * i + j).type());
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
            const bool isKingInCheck(types::PieceColour c) const {
                assert(c != types::PieceColour::NONE);
                player::Player &player = players.at(helper::indexFromColour(c));
                std::set<types::boardIndex> s = player.getPieces().at(helper::indexFromType(types::PieceType::KING)).get();
                auto node = s.extract(s.begin());
                assert(!node.empty());
                types::boardIndex kingIndex = node.value();
                // generate possible attacks
                // check ray attacks () and 
                std::vector<types::Direction> diags {types::Direction::NORTHEAST, types::Direction::SOUTHEAST, types::Direction::SOUTHWEST, types::Direction::NORTHWEST};
                std::vector<types::Direction> upDowns {types::Direction::NORTH, types::Direction::EAST, types::Direction::SOUTH, types::Direction::WEST};
                for (types::Direction d : diags) {
                    // sliding piece check
                    std::array<types::boardIndex, 41> ray = getRay(kingIndex, d);
                    types::boardIndex last = 0;
                    for (unsigned int i = 0; i < ray.size(); ++i) {
                        if (ray.at(i) == 0) {
                            break;
                        }
                        last = i;
                    }
                    if (last != 0) {
                        if (existsEnemyPiece(kingIndex, last) && (boardArray.at(last).type() == types::PieceType::BISHOP || boardArray.at(last).type() == types:: PieceType::QUEEN)) {
                            return true;
                        }
                    }

                    // pawn check
                    auto diagIndex = helper::shiftOne(kingIndex, d);
                    if (existsEnemyPiece(kingIndex, diagIndex) && (boardArray.at(diagIndex).type() == types::PieceType::PAWN)) {
                        // get the pawn shift
                        // if the result of the pawn shift is the kignIndex then return true 
                        std::array<types::boardIndex, 2> pawnAttacks = pawnCaptureShift(diagIndex);
                        for (unsigned int i = 0; i < pawnAttacks.size(); ++i) {
                            types::boardIndex attack = pawnAttacks.at(i);
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
                std::array<types::boardIndex, 41> knightRays = knightShift(kingIndex);
                for (unsigned int i = 0; i < knightRays.size(); ++i) {
                    types::boardIndex index = knightRays.at(i);
                    if (index == 0) {
                        break;
                    }
                    if (existsEnemyPiece(kingIndex, index) && boardArray.at(index).type() == types::PieceType::KNIGHT) {
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
                // segfaults with O3 not O2 
                // segfault here 
                // could be trying to access a move that is deleted
                for (unsigned int i = 0; i < tmp.size(); ++i) {
                    std::unique_ptr<types::Move> &m = tmp.at(i);
                    virtualPlayMove(*m);
                    if (!isKingInCheck(c)) {
                        out.push_back(std::move(m));
                    }
                    virtualUnPlayMove();
                }
                // segfault end
                out.shrink_to_fit();
                return;
            }

            // gets the next colour such that the next colour's player is not checkmate
            // if there are no more players that aren't in checkmate it returns the provided colour
            // takes the colour to increment and the iteration counter
            // iteration should start on 0
            constexpr types::PieceColour getPrevTurn(types::PieceColour c, unsigned int iteration) const {
                if (iteration == 4) {
                    return c;
                }
                switch (c) {
                    case types::PieceColour::BLUE:
                        return players.at(helper::indexFromColour(types::PieceColour::RED)).get().isCheckmate() ? getPrevTurn(types::PieceColour::RED, ++iteration) : types::PieceColour::RED;
                    case types::PieceColour::YELLOW:
                        return players.at(helper::indexFromColour(types::PieceColour::BLUE)).get().isCheckmate() ? getPrevTurn(types::PieceColour::BLUE, ++iteration) : types::PieceColour::BLUE;
                    case types::PieceColour::GREEN:
                        return players.at(helper::indexFromColour(types::PieceColour::YELLOW)).get().isCheckmate() ? getPrevTurn(types::PieceColour::YELLOW, ++iteration) : types::PieceColour::YELLOW;
                    case types::PieceColour::RED:
                        return players.at(helper::indexFromColour(types::PieceColour::GREEN)).get().isCheckmate() ? getPrevTurn(types::PieceColour::GREEN, ++iteration) : types::PieceColour::GREEN;
                    default:
                        return types::PieceColour::NONE;
                }
                return types::PieceColour::NONE;
            }

            // gets the next colour such that the next colour's player is not checkmate
            // if there are no more players that aren't in checkmate it returns the provided colour
            // takes the colour to increment and the iteration counter
            // iteration should start on 0
            constexpr types::PieceColour getNextTurn(types::PieceColour c, unsigned int iteration) const {
                if (iteration == 4) {
                    return c;
                }
                switch (c) {
                    case types::PieceColour::RED:
                        return players.at(helper::indexFromColour(types::PieceColour::BLUE)).get().isCheckmate() ? getNextTurn(types::PieceColour::BLUE, ++iteration) : types::PieceColour::BLUE;
                    case types::PieceColour::BLUE:
                        return players.at(helper::indexFromColour(types::PieceColour::YELLOW)).get().isCheckmate() ? getNextTurn(types::PieceColour::YELLOW, ++iteration) : types::PieceColour::YELLOW;
                    case types::PieceColour::YELLOW:
                        return players.at(helper::indexFromColour(types::PieceColour::GREEN)).get().isCheckmate() ? getNextTurn(types::PieceColour::GREEN, ++iteration) : types::PieceColour::GREEN;
                    case types::PieceColour::GREEN:
                        return players.at(helper::indexFromColour(types::PieceColour::RED)).get().isCheckmate() ? getNextTurn(types::PieceColour::RED, ++iteration) : types::PieceColour::RED;
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
                players.at(pIndex).get().update(m);
                if (m.isCapture()) {
                    for (unsigned int i = 0; i < players.size(); ++i) {
                        if (pIndex != i) {
                            player::Player &p = players.at(i).get();
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
                    boardArray.at(m.toIndex()).setPiece(m.promotionPiece(), m.fromColour());
                    boardArray.at(m.fromIndex()).setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                    
                    moveStack.push(m);
                    return;
                }


                if (m.isCastling()) {
                    int dif = m.toIndex() - m.fromIndex();
                    int absDif = abs(dif);
                    if (absDif == 3 || absDif == 48) {
                        // queenside castle
                        boardArray.at(m.fromIndex() + (dif / 3 * 2)).setPiece(m.fromPiece(), m.fromColour());
                    } else {
                        // kingside castle
                        boardArray.at(m.fromIndex() + dif).setPiece(m.fromPiece(), m.fromColour());
                    }
                    boardArray.at(m.toIndex() - dif).setPiece(m.capturedPiece(), m.capturedColour());
                    boardArray.at(m.fromIndex()).setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                    boardArray.at(m.toIndex()).setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);

                    
                    moveStack.push(m);
                    return;
                }

                // quiet moves
                assert (helper::indexOnBoard(m.fromIndex()) && helper::indexOnBoard(m.toIndex()));
                boardArray.at(m.toIndex()).setPiece(m.fromPiece(), m.fromColour());
                boardArray.at(m.fromIndex()).setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                moveStack.push(m);

            }

            void playMove(types::Move m) {
                // runtime check that everything is working smoothly
                // std::cout << "nuts=" << helper::typeToChar(boardArray.at(m.fromIndex()).type()) << std::endl;
                // std::cout << "nuts=" << helper::typeToChar(m.fromPiece()) << std::endl;
                assert(boardArray.at(m.fromIndex()).type() == m.fromPiece());
                assert(boardArray.at(m.toIndex()).type() == m.capturedPiece());
                // if ((boardArray.at(m.fromIndex()).type() != m.fromPiece()) || (boardArray.at(m.toIndex()).type() != m.capturedPiece())) {
                //     printPaddedBoard();
                //     std::string s = "types::Move is poorly formed: board square does not match move square data. Board type: " 
                //     + helper::typeToChar(boardArray.at(m.fromIndex()).type()) + '(' + std::to_string(m.fromIndex()) + ")" 
                //     + ", " + helper::typeToChar(m.fromPiece()) + "(" + std::to_string(m.fromIndex()) + ")" + "\n";
                //     throw std::invalid_argument(s);
                // }

                assert(boardArray.at(m.fromIndex()).colour() == turn);
                // if (boardArray.at(m.fromIndex()).colour() != turn) {
                //     std::string s = "It is not the given move's turn. Turn: " + helper::colourToChar(turn) 
                //         + 'c' + ':' + ' ' + helper::colourToChar(m.fromColour()) + 'c' +  ':' + ' ' 
                //         + helper::colourToChar(boardArray.at(m.fromIndex()).colour()) + '\n';
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
                        boardArray.at(tmp).setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);
                        tmp = helper::shiftOne(tmp, d);
                    }
                }
                // if (lastMove.isEnPeasant()) {
                //     boardArray.at(helper::shiftOne(lastMove.toIndex(), helper::getUp(lastMove.fromColour()))).setPiece(types::PieceType::EMPTY, types::PieceColour::NONE);                    
                // }
                boardArray.at(lastMove.toIndex()).setPiece(lastMove.capturedPiece(), lastMove.capturedColour()); //return data to state
                boardArray.at(lastMove.fromIndex()).setPiece(lastMove.fromPiece(), lastMove.fromColour()); 
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
    
            inline bool movesPlayed() const {
                return moveStack.size() != 0;
            }            
        
            constexpr types::Move indicesToMove(types::boardIndex from, types::boardIndex to) const {
                types::PieceType promotionType = isPromotion(from, to) ? types::PieceType::QUEEN : types:: PieceType::EMPTY;
            
                bool special = isCastling(from, to) /*|| isEnPeasant(from, to)*/;
                return types::Move(from, to, 0, 
                boardArray.at(from).type(), boardArray.at(from).colour(),
                boardArray.at(to).type(), boardArray.at(to).colour(),
                promotionType, special);
            }
            
            // generates the appropriate shift of the given index
            std::array<types::boardIndex, 41> genShift(types::boardIndex from) {
                switch (boardArray.at(from).type()) {
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

            constexpr bool isPlayerCheckmate(types::PieceColour c) const {
                return players.at(helper::indexFromColour(c)).get().isCheckmate();
            }
    };

    

    
};

#endif