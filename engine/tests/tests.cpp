#include"../types.h"
#include"../helper.h"
#include"../playerData.h"
#include"../board.h"
#include"../engine.h"

#include<catch2/catch_test_macros.hpp>
#include<cstdint>
#include<vector>
#include<set>

using namespace types;
using namespace helper;
using namespace player;
using namespace board;
using namespace engine;

Move emptyMove() {
    return Move();
}
 
Square emptySquare() {
    return Square();
}

TEST_CASE("Type constructions", "[types]") {
    
    Move em = emptyMove();
    CHECK(em.fromIndex() == 300);
    CHECK(em.capturedPiece() == PieceType::EMPTY);

    Square es = emptySquare();
    CHECK_FALSE(es.isAccessible());
    CHECK(es.colour() == PieceColour::NONE);
    CHECK(es.type() == PieceType::EMPTY);

    Square rs = Square(true, PieceType::ROOK, PieceColour::RED);
    CHECK(rs.type() == PieceType::ROOK);
    rs.setPiece(PieceType::BISHOP, PieceColour::RED);
    CHECK(rs.type() == PieceType::BISHOP);
    CHECK(rs.colour() == PieceColour::RED);
}

TEST_CASE("Helper tests", "[helper]") {
    CHECK(GREENUP == Direction::WEST);
    CHECK(REDDOWN == Direction::SOUTH);
    CHECK(YELLOWLEFT == Direction::EAST);
    CHECK(BLUERIGHT == Direction::SOUTH);

    CHECK(indexFromType(PieceType::PAWN) == 0);
    CHECK(indexFromType(PieceType::KNIGHT) == 2); 
    CHECK(indexFromType(PieceType::KING) == 5);

    CHECK(indexFromColour(PieceColour::RED) == 0);
    CHECK(indexFromColour(PieceColour::GREEN) == 3);

    CHECK(getLeft(PieceColour::RED) == Direction::WEST);
    CHECK(getRight(PieceColour::BLUE) == Direction::SOUTH);
    CHECK(getUp(PieceColour::GREEN) == Direction::WEST);
    CHECK(getDown(PieceColour::YELLOW) == Direction::NORTH);

    CHECK(indexInCorners(50));
    CHECK(indexInCorners(33));
    CHECK(indexInCorners(254));
    CHECK_FALSE(indexInCorners(288));
    CHECK_FALSE(indexInCorners(300));
    CHECK_FALSE(indexInCorners(212));

    CHECK(indexOnBoard(212));
    CHECK(indexOnBoard(251));
    CHECK_FALSE(indexOnBoard(300));
    CHECK(indexOnBoard(36));
    
    CHECK_FALSE(isYellowStart(36));
    CHECK_FALSE(isYellowStart(0));
    CHECK(isRedStart(36));
    CHECK_FALSE(isBlueStart(99));
    CHECK(isBlueStart(98));
    CHECK_FALSE(isBlueStart(241));

    CHECK(isRookStart(244));
    CHECK(isPawnStart(228));
    CHECK_FALSE(isKnightStart(300));
    CHECK_FALSE(isRookStart(260));
    CHECK(isEmptyStart(195));

    CHECK(generateSquare(120).type() == PieceType::EMPTY);
    CHECK(generateSquare(251).type() == PieceType::ROOK);
    CHECK(generateSquare(251).colour() == PieceColour::YELLOW);

    vector<boardIndex> v1 = {0, 300, 212, 113, 127};
    vector<boardIndex> v2 = {209, 210};
    vector<boardIndex> v3 = concat(v1, v2);

    CHECK(v3.size() == 7);
    CHECK(v3[0] == 0);
    CHECK(v3[6] == 210);
    CHECK(v3[2] == 212);

    v1 = vector<boardIndex>{0, 300};
    v3 = layer(v1, v2);

    CHECK(v3[0] == 209);
    CHECK(v3[1] == 510);
    CHECK(v3.size() == 2);

    multiplyValues(&v1, (boardIndex) 2);
    CHECK(v1[0] == 0);
    CHECK(v1[1] == 600);
    CHECK(v1.size() == 2);

    CHECK(getPrevTurn(PieceColour::BLUE) == PieceColour::RED);
    CHECK(getNextTurn(PieceColour::YELLOW) == PieceColour::GREEN);
    CHECK(getNextTurn(PieceColour::RED) == PieceColour::BLUE);

    CHECK(getDirection(0, 80) == Direction::NORTH);
    CHECK(getDirection(36, 44) == Direction::EAST);
    CHECK(getDirection(244, 36) == Direction::SOUTH);
    CHECK(getDirection(174, 167) == Direction::WEST);

    CHECK(shiftOne(115, Direction::EAST) == 116);
    CHECK(shiftOne(180, Direction::NORTHEAST) == 197);
    CHECK(shiftOne(220, Direction::SOUTHWEST) == 203);
    CHECK(shiftOne(115, Direction::SOUTH) == 99);
    CHECK(shiftOne(180, Direction::WEST) == 179);
    CHECK(shiftOne(220, Direction::NORTHWEST) == 235);

    CHECK(toIndex(0, 0) == 0);
    CHECK(toIndex(2, 5) == 82);
    CHECK(to16RC(toIndex(12, 2)).first == 12);
    CHECK(to16RC(toIndex(12, 2)).second == 2);
}

TEST_CASE("Player Data tests", "[player]") {
    // PAWN MOVE
    // test must update and revert successfully
    Move m = Move(52, 68, 100, PieceType::PAWN, PieceColour::RED, PieceType::EMPTY, PieceColour::NONE);
    Move em = Move();
    Player r = Player(PieceColour::RED);
    REQUIRE(r.getPieces().size() == 6);
    CHECK(r.getPieces()[0].get().size() == 8);
    r.update(m);
    REQUIRE(r.getPieces()[0].get().contains(68) == r.indexInData(68, PieceType::PAWN));
    CHECK(r.indexHasMoved(68));
    CHECK_FALSE(r.getPieces()[0].get().contains(52));
    r.revert(m, em, em);
    CHECK(r.getPieces()[0].get().contains(52));
    CHECK(r.indexInData(52, PieceType::PAWN));
    CHECK_FALSE(r.getPieces()[0].get().contains(68));
    CHECK_FALSE(r.indexHasMoved(52));
    CHECK_FALSE(r.indexHasMoved(68));

    Player y = Player(PieceColour::YELLOW);
    y.update(m);
    CHECK(y.getPieces()[0].get().size() == 8);
    CHECK_FALSE(y.getPieces()[0].get().contains(68));
    CHECK_FALSE(y.getPieces()[0].get().contains(52));
    CHECK_FALSE(y.indexHasMoved(68));
    CHECK_FALSE(y.indexHasMoved(52));
}

TEST_CASE("Board tests", "[board]") {
    Board b = Board();
    CHECK(b.getCurrentTurn() == PieceColour::RED);
    for (auto c : playableColours) {
        CHECK(b.getCurrentTurn() == c);
        auto moves = b.generateLegalMoves(c);
        CHECK(moves.size() == 20);
        CHECK_FALSE(b.isKingInCheck(c));
        b.playMove(moves[0]);
        auto last = b.getLastMove(c);
        CHECK(last.fromColour() == moves[0].fromColour());
        CHECK(last.fromIndex() == moves[0].fromIndex());
        
        CHECK_FALSE(b.isKingInCheck(c));
    }
    b.unPlayMove();
    PieceColour fromC = b.getLastMove(PieceColour::GREEN).fromColour(); 
    CHECK_FALSE(fromC == PieceColour::GREEN);

}

TEST_CASE("Engine tests", "[engine]") {
    Engine re = Engine(PieceColour::RED);
    Engine be = Engine(PieceColour::BLUE);
    Engine ye = Engine(PieceColour::YELLOW);
    Engine ge = Engine(PieceColour::GREEN);
    
    auto m = re.chooseNextMove();
    REQUIRE(m.fromColour() == PieceColour::RED);
    REQUIRE(m.fromIndex() != 300);
    REQUIRE(m.toIndex() != 300);
    REQUIRE_FALSE(re.evaluateBoard().first == PieceColour::NONE);
    
    
}

TEST_CASE("Engine Process tests", "[engineprocess]") {
    
}