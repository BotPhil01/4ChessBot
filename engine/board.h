#include"helper.h"
#include"playerData.h"
#include"types.h"

#include<cstdint>
#include <sstream>
#include <utility>

#ifndef BOARD_H
#define BOARD_H

#define ROWS 4
#define COLS 5
#define DIRLEN 8
// TODO convert the vector usage to array usage
// TOOD make a fixed size variably filled array
// requires a looping mechanism,
// a size checker (how much of the array is correctly filled)
// a default incorrect value
namespace board {
    // causes namespace pollution
    using std::string_view_literals::operator""sv;

    class BitBoard {
        // bitboard is padded to include empty rows follwoing a 16*18 pattern
        // uses 16 8 20 implementation due to lack of precisely sized words
        // (4*5) * 16 = 16 x 20 board = 320 -> alot less overhead
        // use little endian for board layout
        // so a1 = bits[0][0] & 2^9
        // a5 = bits[1][0] & 2^1
        using bitsType = std::array<std::array<std::uint16_t, ROWS>, COLS>;

        bitsType bits;
        // starting at north and going clockwisee
        const std::array<const std::uint16_t, DIRLEN> overflowMasks = {
            0xF000, 0xF888, 0x8888, 0x888F, 0x000F, 0x111F, 0x1111, 0xF111};
        const std::array<const std::uint16_t, DIRLEN> overflowExcludeMasks = {
            static_cast<std::uint16_t>(~0xF000),
            static_cast<std::uint16_t>(~0xF888),
            static_cast<std::uint16_t>(~0x8888),
            static_cast<std::uint16_t>(~0x888F),
            static_cast<std::uint16_t>(~0x000F),
            static_cast<std::uint16_t>(~0x111F),
            static_cast<std::uint16_t>(~0x1111),
            static_cast<std::uint16_t>(~0xF111)};
        const std::array<const std::uint16_t, DIRLEN> excludeMasks = {
            0x000F, 0x111F, 0x1111, 0xF111, 0xF000, 0xF888, 0x8888, 0x888F};
        // right shifts
        const std::array<const int, DIRLEN> shifts = {4, 5, 1, -3, -4, -5, -1, 3};

        const std::array<const int, DIRLEN> diagCornerMasks = {0x0, 0x8000, 0x0, 0x0008, 0x0, 0x0001, 0x0, 0x1000};
        const std::array<const int, DIRLEN> diagCornerShifts = {0, -15, 0, 9, 0, 15, 0, -9};
        const std::array<const int, DIRLEN> diagHoriMasks = {0x0, 0x0888, 0x8888, 0x8880, 0x0, 0x1110, 0x1111, 0x0111};
        const std::array<const int, DIRLEN> diagHoriShifts = {0, 1, 0, -7, 0, -1, 0, 7};
        const std::array<const int, DIRLEN> diagVertMasks = {0xF000, 0x7000, 0x0, 0x0007, 0x000F, 0x0007, 0x0, 0x7000};
        const std::array<const int, DIRLEN> diagVertShifts = {-12, -11, 0, 13, 12, 11, 0, -13};

        const std::array<const int, DIRLEN> vertIndex = {-1, -1, 0, 1, 1, 1, 0, -1};
        const std::array<const int, DIRLEN> horiIndex = {0, 1, 1, 1, 0, -1, -1, -1};

        // checks if there is a vertical carry over that needs to be considered
        // returns 0 if no carry over is required
        // returns 1 if a northwards carry over is required
        // returns 2 if a southwards carry over is required
        int vertCarryOverCheck(types::Direction dir, int rowIndex) {
            switch(dir) {
                case types::Direction::NORTH: case types::Direction::NORTHWEST: case types::Direction::NORTHEAST:
                    return rowIndex > 0;
                case types::Direction::SOUTH: case types::Direction::SOUTHWEST: case types::Direction::SOUTHEAST:
                    return rowIndex < ROWS - 1;
                default:
                    return 0;
            }
        }

        // checks if there is a vertical carry over that needs to be considered
        // returns 0 if no carry over is required
        // returns 1 if a eastwards carry over is required
        // returns 2 if a westwards carry over is required
        int horiCarryOverCheck(types::Direction dir, int colIndex) {
            switch(dir) {
                case types::Direction::NORTHEAST: case types::Direction::SOUTHEAST: case types::Direction::EAST: 
                    return colIndex < COLS - 1;
                case types::Direction::NORTHWEST: case types::Direction::SOUTHWEST: case types::Direction::WEST: 
                    return colIndex > 0;
                default:
                    return 0;
            }
        }

        // shifts the whole bitboard in one direction
        // accounts for overeflow between constituent parts
        void shiftOne(const types::Direction dir) {
            for (int rowIndex = 0; rowIndex < ROWS; rowIndex++) {
                for (int colIndex = 0; colIndex < COLS; colIndex++){
                    const int dirIndex = helper::indexFromDirection(dir);
                    const std::uint16_t subBoard = bits[rowIndex][colIndex];
                    // carry over
                    const std::uint16_t corner = (subBoard & diagCornerMasks[dirIndex]) >> diagCornerShifts[dirIndex];
                    const std::uint16_t vert = (subBoard & diagVertMasks[dirIndex]) >> diagVertShifts[dirIndex];
                    const std::uint16_t hori = (subBoard & diagHoriMasks[dirIndex]) >> diagHoriShifts[dirIndex];
                    const int newRowIndex = rowIndex + horiIndex[dirIndex];
                    const int newColIndex = colIndex + vertIndex[dirIndex];
                    bits[newRowIndex][newColIndex] |= corner;
                    bits[newRowIndex][colIndex] |= hori;
                    bits[rowIndex][newColIndex] |= vert;
                    // change original bitboard
                    bits[rowIndex][colIndex] = (subBoard >> shifts[dirIndex]) ^ excludeMasks[dirIndex];
                }
            }
        }

        // uses little endianness for everythin
        void print() {
            std::array<std::ostringstream, ROWS * 4> tmp;

            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    // convert std::uint16_t into 4x4 word
                    for (int a = 0; a < 4; a++) {
                        for (int b = 0; b < 4; b++) {
                            const std::uint16_t isolator = 1;
                            std::uint16_t isolatedBit = bits[ROWS][COLS] & (isolator >> (a * 4 + b));
                            tmp[ROWS * 4 + a] << (isolatedBit ? '1' : '0');
                        }
                    }
                }
            }
            for (auto it = tmp.end(); it >= tmp.begin(); it++) {
                std::cout << (*it).str() << '\n';
            }
        }
    };
};

#endif
