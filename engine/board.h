#include"helper.h"
#include"playerData.h"
#include"types.h"

#include<cstdint>
#include<sstream>
#include <string_view>
#include<utility>

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
    using bitsType = std::array<std::array<std::uint16_t, COLS>, ROWS>;

    class BitBoard {
        bitsType bits;
        public:
        BitBoard(bitsType _bits) : bits(_bits)
        {
        }
        // bitboard is padded to include empty rows follwoing a 16*18 pattern
        // uses 16 8 20 implementation due to lack of precisely sized words
        // (4*5) * 16 = 16 x 20 board = 320 -> alot less overhead
        // use little endian for board layout
        // so a1 = bits[0][0] & 2^9
        // a5 = bits[1][0] & 2^1

        // starting at north and going clockwisee
        const std::array<const std::uint16_t, DIRLEN> excludeMasks = {0xFFF0, 0, 0xEEEE, 0, 0x0FFF, 0, 0x7777, 0};
        // lshifts
        // const std::array<const int, DIRLEN> shifts = {4, 5, 1, -3, -4, -5, -1, 3};
        const std::array<const int, DIRLEN> shifts = {4, 0, 1, 0, -4, 0, -1, 0};

        const std::array<const int, DIRLEN> cornerMasks = {0x0, 0x8000, 0x0, 0x0008, 0x0, 0x0001, 0x0, 0x1000};
        const std::array<const int, DIRLEN> cornerShifts = {0, -15, 0, 9, 0, 15, 0, -9};
        // const std::array<const int, DIRLEN> horiMasks = {0x0, 0x0888, 0x8888, 0x8880, 0x0, 0x1110, 0x1111, 0x0111};
        const std::array<const int, DIRLEN> horiMasks = {0, 0, 0x8888, 0, 0, 0, 0x1111, 0};
        // const std::array<const int, DIRLEN> horiShifts = {0, 1, -3, -7, 0, -1, 3, 7};
        const std::array<const int, DIRLEN> horiShifts = {0, 0, -3, 0, 0, 0, 3, 0};
        // const std::array<const int, DIRLEN> vertMasks = {0xF000, 0x7000, 0x0, 0x0007, 0x000F, 0x0007, 0x0, 0x7000};
        // const std::array<const int, DIRLEN> vertShifts = {-12, -11, 0, 13, 12, 11, 0, -13};
        const std::array<const int, DIRLEN> vertMasks = {0xF000, 0, 0, 0, 0x000F, 0, 0, 0};
        const std::array<const int, DIRLEN> vertShifts = {-12, 0, 0, 0, 12, 0, 0, 0};

        const std::array<const int, DIRLEN> vertIndex = {-1, -1, 0, 1, 1, 1, 0, -1};
        const std::array<const int, DIRLEN> horiIndex = {0, 1, 1, 1, 0, -1, -1, -1};

        // same as >> but with defined behaviour for - values
        std::uint16_t rShift(const std::uint16_t val, const int shift) {
            if (shift < 0) {
                return lShift(val, -shift);
            }
            return val >> shift;
        }

        // same as << but with defined behaviour for - values
        std::uint16_t lShift(const std::uint16_t val, const int shift) {
            if (shift < 0) {
                return rShift(val, -shift);
            }
            return val << shift;
        }

        // shifts the whole bitboard in one direction
        // accounts for overeflow between constituent parts
        void shiftOne(const types::Direction dir) {
            switch (dir) {
                case types::Direction::NORTHEAST:
                    shiftOne(types::Direction::NORTH);
                    shiftOne(types::Direction::EAST);
                    break;
                case types::Direction::SOUTHEAST:
                    shiftOne(types::Direction::SOUTH);
                    shiftOne(types::Direction::EAST);
                    break;
                case types::Direction::SOUTHWEST:
                    shiftOne(types::Direction::SOUTH);
                    shiftOne(types::Direction::WEST);
                    break;
                case types::Direction::NORTHWEST:
                    shiftOne(types::Direction::NORTH);
                    shiftOne(types::Direction::WEST);
                    break;
                default:
                    bitsType overflows = {{
                        {{0, 0, 0, 0, 0}},
                        {{0, 0, 0, 0, 0}},
                        {{0, 0, 0, 0, 0}},
                        {{0, 0, 0, 0, 0}},
                    }};
                    const int dirIndex = helper::indexFromDirection(dir);
                    for (int rowIndex = 0; rowIndex < ROWS; rowIndex++) {
                        for (int colIndex = 0; colIndex < COLS; colIndex++) {
                            const std::uint16_t subBoard = bits[rowIndex][colIndex];
                            // carry over
                            // const std::uint16_t corner = lShift(subBoard & cornerMasks[dirIndex], cornerShifts[dirIndex]);
                            const std::uint16_t vert = lShift(subBoard & vertMasks[dirIndex], vertShifts[dirIndex]);
                            const std::uint16_t hori = lShift(subBoard & horiMasks[dirIndex], horiShifts[dirIndex]);
                            const int newRowIndex = rowIndex + vertIndex[dirIndex];
                            const int newColIndex = colIndex + horiIndex[dirIndex];

                            if (newRowIndex >= 0 && newRowIndex < ROWS) {
                                overflows[newRowIndex][colIndex] |= vert;
                            }
                            if (newColIndex >= 0 && newColIndex < COLS) {
                                overflows[rowIndex][newColIndex] |= hori;
                            }
                            // change original bitboard
                            bits[rowIndex][colIndex] = lShift(subBoard, shifts[dirIndex]) & excludeMasks[dirIndex];
                        }
                    }
                    for (int rowIndex = 0; rowIndex < ROWS; rowIndex++) {
                        for (int colIndex = 0; colIndex < COLS; colIndex++) {
                            bits[rowIndex][colIndex] |= overflows[rowIndex][colIndex];
                        }
                    }
            }
        }

        // uses little endianness for everythin
        void print(std::string_view header = "BitBoard") {
            const int iBoardDimensions = 4;
            std::array<std::ostringstream, ROWS * iBoardDimensions> tmp;

            for (int oRowIndex = 0; oRowIndex < ROWS; oRowIndex++) {
                for (int oColIndex = 0; oColIndex < COLS; oColIndex++) {
                    const std::uint16_t subBoard = bits[oRowIndex][oColIndex];
                    for (int i = 0; i < iBoardDimensions; i++) {
                        const std::uint16_t rowBits = 0xF000 & (subBoard << (iBoardDimensions * i));
                        for (int j = 0; j < iBoardDimensions; j++) {
                            // get each least significant bit in the row and add to print object
                            const int lsb = 0x1000 & (rowBits >> j);
                            tmp[oRowIndex * iBoardDimensions + i] << (lsb ? '1' : '0');
                        }
                    }
                }
            }
            

            std::cout << "------"sv;
            std::cout << header;
            std::cout << "------\n"sv;
            for (unsigned int i = 0; i < tmp.size(); i++) {
                std::cout << tmp[i].str() << '\n';
            }
            std::cout << "--------------------\n"sv;
            std::cout.flush();
        }
    };
};

#endif
