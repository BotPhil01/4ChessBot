#include "board.h"

#define ROWS 4
#define COLS 5
int main() {
    std::array<std::array<std::uint16_t, COLS>, ROWS> arr = {{
        {{0x0021, 0xf000, 0xf000, 0xf000, 0xf000}},
            {{0xf001, 0x1488, 0xf000, 0xf000, 0xf000}},
            {{0xf000, 0xf000, 0xf000, 0xf000, 0xf000}},
            {{0xf000, 0xf000, 0xf000, 0xf000, 0xf000}}
    }};
    board::BitBoard bitboard = {arr};
    bitboard.print();
    bitboard.shiftOne(types::Direction::NORTH);
    bitboard.print("NORTH");
    bitboard.shiftOne(types::Direction::NORTHEAST);
    bitboard.print("NORTHEAST");
    bitboard.shiftOne(types::Direction::EAST);
    bitboard.print("EAST");
    bitboard.shiftOne(types::Direction::SOUTHEAST);
    bitboard.print("SOUTHEAST");
    bitboard.shiftOne(types::Direction::SOUTH);
    bitboard.print("SOUTH");
    bitboard.shiftOne(types::Direction::SOUTHWEST);
    bitboard.print("SOUTHWEST");
    bitboard.shiftOne(types::Direction::WEST);
    bitboard.print("WEST");
    bitboard.shiftOne(types::Direction::NORTHWEST);
    bitboard.print("NORTHWEST");
}
