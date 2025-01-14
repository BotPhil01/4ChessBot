#include"../types.h"
#include<catch2/catch_test_macros.hpp>
#include<cstdint>

using namespace types;

Move emptyMove() {
    return Move();
}
 
Square emptySquare() {
    return Square();
}

TEST_CASE( "Type constructions", "[types]" ) {
    REQUIRE(emptyMove().fromIndex() == 300);
    REQUIRE(emptyMove().capturedPiece() == PieceType::EMPTY);
    REQUIRE_FALSE(emptySquare().isAccessible());
    REQUIRE(emptySquare().colour() == PieceColour::NONE);
    REQUIRE(emptySquare().type() == PieceType::EMPTY);
}