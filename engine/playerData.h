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
            Colour clr;
            
            set<boardIndex> pawns;
            set<boardIndex> rooks;
            set<boardIndex> knights;
            set<boardIndex> bishops;
            set<boardIndex> queens;
            set<boardIndex> kings;
            set<boardIndex> movedPieces;
            // for iteration through pieces
            vector<reference_wrapper<set<boardIndex>>> pieces;
            

            set<Move> playedMoves;
            void genPawns() {
                int lo = 0;
                int hi = 0;
                switch(clr) {
                    case RED:
                        lo = 52;
                        hi = 60;
                        break;
                    case YELLOW:
                        lo = 228;
                        hi = 236;
                        break;
                    case BLUE:
                        lo = 82;
                        hi = 210;
                        break;
                    case GREEN:
                        lo = 93;
                        hi = 221;
                        break;
                    default:
                        lo = 0;
                        hi = 0;
                        break;
                }
                if (clr == RED || clr == YELLOW) {
                    for (; lo < hi; ++lo) {
                        pawns.emplace(lo);
                    }
                } else if(clr == BLUE || clr == GREEN) {
                    for (; lo < hi; lo = shiftOne(lo, NORTH)) {
                        pawns.emplace(lo);
                    }
                }
            }
            void genRooks() {
                switch(clr) {
                    case RED:
                        rooks.emplace(36);
                        rooks.emplace(43);
                        break;
                    case BLUE:
                        rooks.emplace(81);
                        rooks.emplace(193);
                        break;
                    case YELLOW:
                        rooks.emplace(244);
                        rooks.emplace(251);
                        break;
                    case GREEN:
                        rooks.emplace(94);
                        rooks.emplace(206);
                        break;
                    default:
                        return;
                }
            }
            void genKnights() {
                switch(clr) {
                    case RED:
                        knights.emplace(37);
                        knights.emplace(42);
                        break;
                    case BLUE:
                        knights.emplace(97);
                        knights.emplace(177);
                        break;
                    case YELLOW:
                        knights.emplace(245);
                        knights.emplace(250);
                        break;
                    case GREEN:
                        knights.emplace(110);
                        knights.emplace(190);
                        break;
                    default:
                        return;
                }
            }
            void genBishops() {
                switch(clr) {
                    case RED:
                        bishops.emplace(38);
                        bishops.emplace(41);
                        break;
                    case BLUE:
                        bishops.emplace(113);
                        bishops.emplace(161);
                        break;
                    case YELLOW:
                        bishops.emplace(246);
                        bishops.emplace(249);
                        break;
                    case GREEN:
                        bishops.emplace(126);
                        bishops.emplace(174);
                        break;
                    default:
                        return;
                }
            }
            void genQueens() {
                switch(clr) {
                    case RED:
                        queens.emplace(39);
                        break;
                    case BLUE:
                        queens.emplace(129);
                        break;
                    case YELLOW:
                        queens.emplace(248);
                        break;
                    case GREEN:
                        queens.emplace(158);
                        break;
                    default:
                        return;
                }
            }
            void genKings() {
                switch(clr) {
                    case RED:
                        kings.emplace(40);
                        break;
                    case BLUE:
                        kings.emplace(145);
                        break;
                    case YELLOW:
                        kings.emplace(247);
                        break;
                    case GREEN:
                        kings.emplace(142);
                        break;
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
            PieceType getSetType(set<boardIndex> & address) {
                int i = 0;
                for (; i < pieces.size(); ++i) {
                    auto s = pieces.at(i);
                    if (s.get() == address) {
                        break;
                    }
                }
                assert(i < pieces.size());
                return playablePieces[i];
            }
            
            // for updating an index from src to trgt
            // src is assumed to be in our control
            // used in updating for a move
            void handleMoved(boardIndex src, boardIndex trgt) {
                auto it = movedPieces.find(src);
                if (it != movedPieces.end()) {
                    movedPieces.emplace_hint(it, trgt);
                    movedPieces.extract(it);
                    return;
                }
                movedPieces.emplace(trgt);
            }

            
            // for removing an index i and adding depending on its prior move
            // used in reverting a move
            // NOTE i should not equal last.toIndex();
            // i is assumed to be in our control
            void handleMoved(boardIndex i, Move last) {
                assert(i != last.toIndex()); // bad usage
                auto it = movedPieces.find(i); 
                assert(it != movedPieces.end());
                if (last.toIndex() != 300) {
                    // has prior movement history
                    movedPieces.emplace_hint(it, last.toIndex());
                }
                movedPieces.extract(it);
                
            }   
        public:
            bool canPlai = false;
            Player(Colour c) :
            clr(c) {
                genPieces();                
            }

            PieceColour colour() {
                return clr;
            }

            const vector<reference_wrapper<set<boardIndex>>> getPieces() {
                return pieces;
            }

            // updates data to be consistent with a move
            // strategy design
            // returns the captured piece type if a capture was handled else empty piecetype
            PieceType update(Move m) {
                // m = fromI = 148, toI: = 132, fromC = types::PieceColour::YELLOW, fromP = types::PieceType::PAWN, capturedC = types::PieceColour::NONE, capturedP = types::PieceType::EMPTY, special = false, 
                // toP = types:PieceType::QUEEN, totalMoves = 0

                // check whether we are being taken or taking someone else
                set<boardIndex> &fromSet = pieces[indexFromType(m.fromPiece())].get();
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
                    set<boardIndex> &toSet = pieces[indexFromType(m.promotionPiece())].get();
                    // remove moved piece

                    fromSet.extract(fromIt);
                    toSet.emplace(m.toIndex());

                    // update moved pieces list
                    auto it = movedPieces.find(m.fromIndex());
                    if (it != movedPieces.end()) {
                        movedPieces.extract(it);
                    }
                    movedPieces.emplace(m.toIndex());

                } else if (m.isEnPeasant()) {
                    boardIndex changeTo = shiftOne(m.toIndex(), getUp(m.fromColour()));
                    fromSet.emplace_hint(fromIt, changeTo);
                    fromSet.extract(fromIt);

                } else if (m.isCastling()) {
                    int dif = m.toIndex() - m.fromIndex();
                    int absDif = abs(dif);
                    if (absDif == 3 || absDif == 48) {
                        // queenside castle
                        fromSet.emplace_hint(fromIt, m.fromIndex() + (dif / 3 * 2));
                    } else {
                        // kingside castle
                        fromSet.emplace_hint(fromIt, m.fromIndex() + dif);
                    }
                    fromSet.extract(fromIt);
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
            // takes a move to remove and the previous moves of the involved indices
            void revert(Move m, Move &fromPrev, Move &captPrev) {
                if (m.fromColour() != clr && !m.isCapture()) {
                    return;
                }
                if (m.fromColour() != clr && m.isCapture()) {
                    // our piece could've been taken
                    // if this triggers then m has not been set properly after being played
                    assert(
                        m.capturedPiece() != PieceType::EMPTY || m.capturedPiece() != PieceType::BLOCK
                        && m.capturedColour() != PieceColour::NONE
                    );
                    
                    if (m.capturedColour() == clr) { 
                        // our piece is returned
                        set<boardIndex> &pieceSet = pieces[indexFromType(m.capturedPiece())].get();
                        pieceSet.emplace(m.toIndex());
                        if (captPrev.toIndex() == 300) {
                            return;                    
                        }
                        movedPieces.emplace(m.toIndex());
                    }
                    
                    return;
                }
                if (m.isPromotion()) {
                    // remove from promoted set 
                    set<boardIndex> &toSet = pieces[indexFromType(m.promotionPiece())].get();
                    auto toIt = toSet.find(m.toIndex());
                    assert(toIt != toSet.end());
                    toSet.extract(toIt);
                    // return to original set
                    set<boardIndex> &fromSet = pieces[indexFromType(m.fromPiece())].get();
                    fromSet.emplace(m.fromIndex());

                    if (fromPrev.fromIndex() == 300) {
                        return;
                    }
                    handleMoved(m.toIndex(), fromPrev);
                    return;
                } else if (m.isCastling()) {
                    assert(m.fromColour() == clr);
                    // clear both rook and king squares
                    boardIndex tmp = m.fromIndex();
                    Direction d = getDirection(m.fromIndex(), m.toIndex());
                    bool rookFound = false;
                    bool kingFound = false;
                    while (tmp != m.toIndex() && !(rookFound && kingFound)) {
                        auto tmpIt = rooks.find(tmp);
                        if (tmpIt != rooks.end()) {
                            // found our rook
                            handleMoved(tmp, captPrev);
                            rooks.emplace_hint(tmpIt, m.toIndex());
                            rooks.extract(tmpIt);
                            rookFound = true;
                        }
                        tmpIt = kings.find(tmp);
                        if (tmpIt != kings.end()) {
                            handleMoved(tmp, fromPrev);
                            kings.emplace_hint(tmpIt, m.toIndex());
                            kings.extract(tmpIt);
                            kingFound = true;
                        }
                        tmp = shiftOne(tmp, d);
                    }
                } else if (m.isEnPeasant()) {
                    boardIndex remove = shiftOne(m.toIndex(), getUp(m.fromColour()));
                    auto it = pawns.find(remove);
                    // the pawn to be removed must exist
                    assert(it != pawns.end());
                    pawns.emplace_hint(it, m.fromIndex());
                    pawns.extract(it);
                    handleMoved(remove, fromPrev);
                } else {
                    // change index
                    set<boardIndex> &pieceSet = pieces[indexFromType(m.fromPiece())].get();
                    auto it = pieceSet.find(m.toIndex());
                    assert(it != pieceSet.end());
                    pieceSet.extract(it);
                    pieceSet.emplace(m.fromIndex());

                    handleMoved(m.toIndex(), fromPrev);
                    return;
                }
            }
            
            // returns whether in the current game state the index is in the players game data
            bool indexInData(boardIndex i, PieceType p) {
                set<boardIndex> &pieceSet = pieces[indexFromType(p)].get();
                return pieceSet.contains(i);
            }

            bool indexHasMoved(boardIndex i) {
                return (movedPieces.find(i) != movedPieces.end());
            }

    };
}

#endif