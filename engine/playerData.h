#include"helper.h"
#include"types.h"

#include<vector>
#include<set>
#include<functional>
#ifndef PLAYERDATA_H
#define PLAYERDATA_H



namespace player {    
    using namespace std;
    using namespace helper;
    using namespace types;
    using Colour = PieceColour;
    class Player {
        private: 
            Colour colour;
            set<boardIndex> pawns;
            set<boardIndex> rooks;
            set<boardIndex> knights;
            set<boardIndex> bishops;
            set<boardIndex> queens;
            set<boardIndex> kings;
            set<boardIndex> movedPieces;
            // for iteration through pieces
            vector<const reference_wrapper<set<boardIndex>>> pieces;
            

            set<Move> playedMoves;
            void genPawns() {
                int lo = 0;
                int hi = 0;
                switch(colour) {
                    case RED:
                        lo = 52;
                        hi = 60;
                    case YELLOW:
                        lo = 228;
                        hi = 236;
                    case BLUE:
                        lo = 81;
                        hi = 210;
                    case GREEN:
                        lo = 93;
                        hi = 221;
                    default:
                        lo = 0;
                        hi = 0;
                }
                if (colour == RED || colour == YELLOW) {
                    for (; lo < hi; ++lo) {
                        pawns.emplace(lo);
                    }
                } else if(colour == BLUE || colour == GREEN) {
                    for (; lo < hi; lo = shiftOne(lo, NORTH)) {
                        pawns.emplace(lo);
                    }
                }
            }
            void genRooks() {
                switch(colour) {
                    case RED:
                        rooks.emplace(36);
                        rooks.emplace(43);
                    case BLUE:
                        rooks.emplace(81);
                        rooks.emplace(193);
                    case YELLOW:
                        rooks.emplace(244);
                        rooks.emplace(251);
                    case GREEN:
                        rooks.emplace(94);
                        rooks.emplace(206);
                    default:
                        return;
                }
            }
            void genKnights() {
                switch(colour) {
                    case RED:
                        knights.emplace(37);
                        knights.emplace(42);
                    case BLUE:
                        knights.emplace(97);
                        knights.emplace(177);
                    case YELLOW:
                        knights.emplace(245);
                        knights.emplace(250);
                    case GREEN:
                        knights.emplace(110);
                        knights.emplace(190);
                    default:
                        return;
                }
            }
            void genBishops() {
                switch(colour) {
                    case RED:
                        bishops.emplace(38);
                        bishops.emplace(41);
                    case BLUE:
                        bishops.emplace(113);
                        bishops.emplace(161);
                    case YELLOW:
                        bishops.emplace(246);
                        bishops.emplace(249);
                    case GREEN:
                        bishops.emplace(126);
                        bishops.emplace(174);
                    default:
                        return;
                }
            }
            void genQueens() {
                switch(colour) {
                    case RED:
                        queens.emplace(39);
                    case BLUE:
                        queens.emplace(129);
                    case YELLOW:
                        queens.emplace(248);
                    case GREEN:
                        queens.emplace(158);
                    default:
                        return;
                }
            }
            void genKings() {
                switch(colour) {
                    case RED:
                        kings.emplace(40);
                    case BLUE:
                        kings.emplace(145);
                    case YELLOW:
                        kings.emplace(247);
                    case GREEN:
                        kings.emplace(142);
                    default:
                        return;
                }
            }
            void genPieces() {

                // assigns objects into vectors
                genPawns();
                genRooks();
                genKnights();
                genBishops();
                genQueens();
                genKings();

                pieces = {ref(pawns), ref(rooks), ref(knights), ref(bishops), ref(queens), ref(kings)};
            }
        
            // gets the piecetype that the set is handling
            PieceType getSetType(set<boardIndex> & const address) {
                int i = 0;
                for (; i < pieces.size(); ++i) {
                    auto s = pieces.at(i);
                    if (s.get() == address) {
                        break;
                    }
                }
                assert(i < pieces.size());
                return playableTypes[i];
            }
        
        public:
            Player(Colour c) :
            colour(c) {
                genPieces();                
            }

            const vector<const reference_wrapper<set<boardIndex>>> getPieces() {
                return pieces;
            }

            

            // updates data to be consistent with a move
            // strategy design
            // returns true if and only if the move ended with our piece being captured
            PieceType update(Move m) {
                // check whether we are being taken or taking someone else
                set<boardIndex> fromSet = pieces[indexFromType(m.fromPiece())];
                auto fromIt = fromSet.find(m.fromIndex()); // Log(n)

                if (fromIt == fromSet.end() && !m.isCapture()) {
                    // someone elses move
                    return PieceType::EMPTY;

                } else if (fromIt == fromSet.end() && m.isCapture()) {
                    // our piece could be getting taken
                    // check our sets for the taken piece
                    for (set<boardIndex> &pieceSet : pieces) {
                        auto iterator = pieceSet.find(m.toIndex());
                        if (iterator != pieceSet.end()) {
                            // found our piece
                            pieceSet.extract(iterator);
                            // remove movedPieces index
                            iterator = movedPieces.find(m.toIndex());
                            if (iterator != movedPieces.end()) {
                                movedPieces.extract(iterator);
                            }
                            return getSetType(pieceSet);
                        }
                    }

                } else if(m.isPromotion()) {
                    // we are moving + promoting
                    set<boardIndex> toSet = pieces[indexFromType(m.toPiece())];
                    // remove moved piece

                    fromSet.erase(fromIt);
                    toSet.emplace(m.toIndex());

                    // update moved pieces list
                    auto it = movedPieces.find(m.fromIndex());
                    if (it != movedPieces.end()) {
                        movedPieces.extract(it);
                    }
                    movedPieces.emplace(m.toIndex());

                } else {
                    // we are moving 
                    fromSet.extract(fromIt);
                    fromSet.emplace(m.toIndex());
                    // update moved pieces list
                    auto it = movedPieces.find(m.fromIndex());
                    if (it != movedPieces.end()) {
                        movedPieces.extract(it);
                    }
                    movedPieces.emplace(m.toIndex());
                }

                return PieceType::EMPTY;
            }
            // updates data to remove a move
            // takes a move to remove and a bool indicating whether the moved index after returning was moved at that time
            void revert(Move m, bool movedBefore) {
                if (m.fromColour() != colour && !m.isCapture()) {
                    return;
                }
                if (m.fromColour() != colour && m.isCapture()) {
                    // our piece could've been taken
                    // if this triggers then m has not been set properly after being played
                    assert(
                        m.capturedPiece() != PieceType::EMPTY || m.capturedPiece() != PieceType::BLOCK
                        && m.capturedColour() != PieceColour::NONE);
                    
                    if (m.capturedColour() == colour) { 
                        // our piece is returned
                        auto pieceSet = pieces[indexFromType(m.capturedPiece())];
                        pieceSet.get().emplace(m.fromIndex());
                        
                    }
                    
                    return;
                }
                if (m.fromColour() == colour && m.isPromotion()) {
                    // remove from promoted set 
                    set<boardIndex> &toSet = pieces[indexFromType(m.toPiece())];
                    auto fromIt = toSet.find(m.fromIndex());
                    assert(fromIt != toSet.end());
                    toSet.extract(fromIt);
                    // return to original set
                    set<boardIndex> &fromSet = pieces[indexFromType(m.fromPiece())];
                    fromSet.emplace(m.fromIndex());
                    return;
                }
                if (m.fromColour() == colour) {
                    // change index
                    set<boardIndex> &pieceSet = pieces[indexFromType(m.fromPiece())];
                    auto it = pieceSet.find(m.toIndex());
                    assert(it != pieceSet.end());
                    pieceSet.extract(it);
                    pieceSet.emplace(m.fromIndex());
                    return;
                }
            }


    };
}

#endif