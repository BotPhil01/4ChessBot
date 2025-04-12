#ifndef BITBOARD_H
#define BITBOARD_H

#include <algorithm>
#include<array>
#include<cstdint>
#include <sstream>
#include<string_view>
#include<iostream>
#include<limits.h>
#include "boardDefaults.h"
#include"types.h"
#include"helper.h"

#define ROWS 4
#define COLS 4
#define DIRLEN 8
namespace bitboard {
    using BitsType = std::array<std::array<std::uint16_t, COLS>, ROWS>;
    // starting at north and going clockwisee
    const std::array<const std::uint16_t, DIRLEN> excludeMasks = {0xFFF0, 0, 0xEEEE, 0, 0x0FFF, 0, 0x7777, 0};
    // lshifts
    const std::array<const int, DIRLEN> shifts = {4, 0, 1, 0, -4, 0, -1, 0};

    const std::array<const int, DIRLEN> cornerMasks = {0x0, 0x8000, 0x0, 0x0008, 0x0, 0x0001, 0x0, 0x1000};
    const std::array<const int, DIRLEN> cornerShifts = {0, -15, 0, 9, 0, 15, 0, -9};
    const std::array<const int, DIRLEN> horiMasks = {0, 0, 0x8888, 0, 0, 0, 0x1111, 0};
    const std::array<const int, DIRLEN> horiShifts = {0, 0, -3, 0, 0, 0, 3, 0};
    const std::array<const int, DIRLEN> vertMasks = {0xF000, 0, 0, 0, 0x000F, 0, 0, 0};
    const std::array<const int, DIRLEN> vertShifts = {-12, 0, 0, 0, 12, 0, 0, 0};

    const std::array<const int, DIRLEN> vertIndex = {-1, -1, 0, 1, 1, 1, 0, -1};
    const std::array<const int, DIRLEN> horiIndex = {0, 1, 1, 1, 0, -1, -1, -1};

    void printValues(BitsType bits, std::string_view header = "BitBoard") {
        using namespace std::string_view_literals;
        std::cout << "    "sv << header << "    \n";
        for (int i = 0; i < ROWS; i++) {
            std::cout << "{{"sv;
            for (int j = 0; j < COLS; j++) {
                std::cout << "0x"sv << std::hex << bits[i][j];
                if (j != COLS - 1) {
                    std::cout << ", "sv;
                }
            }

            std::cout << "}},\n"sv;
        }
    }

    // uses little endianness for everythin
    void print(BitsType bits, std::string_view header = "BitBoard") {
        const int iBoardDimensions = 4;
        std::array<std::stringstream, ROWS * iBoardDimensions> tmp;

        for (int oRowIndex = 0; oRowIndex < ROWS; oRowIndex++) {
            for (int oColIndex = 0; oColIndex < COLS; oColIndex++) {
                const std::uint16_t subBoard = bits[oRowIndex][oColIndex];
                for (int i = 0; i < iBoardDimensions; i++) {
                    const std::uint16_t rowBits = 0xF000 & (subBoard << (iBoardDimensions * i));
                    for (int j = 0; j < iBoardDimensions; j++) {
                        // get each least significant bit in the row and add to print object
                        const int lsb = 0x1000 & (rowBits >> j);
                        tmp[oRowIndex * iBoardDimensions + i] << (lsb ? '1' : '0');
                        if ((oColIndex == 0 && tmp[oRowIndex * iBoardDimensions + i].str().length() == 1)
                                || (oColIndex == COLS - 1 && tmp[oRowIndex * iBoardDimensions + i].str().length() == 16)) {
                            tmp[oRowIndex * iBoardDimensions + i] << '|';
                        }
                    }
                }
            }
        }


        using namespace std::string_view_literals;
        std::cout << "    "sv;
        std::cout << header;
        std::cout << "    \n"sv;
        for (unsigned int i = 0; i < tmp.size(); i++) {
            std::cout << tmp[i].str() << '\n';
            if (i == 0 || i == tmp.size() - 2) {
                std::cout << "-|--------------|-\n"sv;
            }
        }
        std::cout << "            \n"sv;
        std::cout.flush();
    }

    typedef struct coords {
        int outerX;
        int outerY;
        int innerX;
        int innerY;
    } coords;

    void boundsCheck(BitsType _bits);
    namespace 
    {
        std::uint16_t lShift(const std::uint16_t val, const int shift);
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

        // rotates 90 degrees anticlockwise about center
        std::uint16_t rotateWord(const std::uint16_t word, const int count) {
            std::uint16_t tmp = 0;
            std::array<std::uint16_t, 16> map = {
                0x0008, 0x0080, 0x0800, 0x8000,
                0x0004, 0x0040, 0x0400, 0x4000,
                0x0002, 0x0020, 0x0200, 0x2000,
                0x0001, 0x0010, 0x0100, 0x1000,
            };
            for (int i = 0; i < 16; i++) {
                // isolate each bit and get mapped value
                const std::uint16_t bit = 1 & (word >> i);
                if (bit) {
                    tmp |= map[i];
                }
            }
            return tmp;
        }


        BitsType bitsApplyOr(BitsType bits1, const BitsType bits2) {
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    bits1[i][j] |= bits2[i][j];
                }
            }
            return bits1;
        }

        BitsType bitsApplyOrWord(BitsType _bits, const std::uint16_t word) {
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    _bits[i][j] |= word;
                }
            }
            return _bits;
        }

        BitsType bitsApplyAnd(BitsType bits1, const BitsType bits2) {
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    bits1[i][j] &= bits2[i][j];
                }
            }
            return bits1;
        }

        BitsType bitsApplyAndWord(BitsType _bits, const std::uint16_t word) {
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    _bits[i][j] &= word;
                }
            }
            return _bits;
        }

        BitsType bitsApplyNot(BitsType _bits) {
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    _bits[i][j] = ~_bits[i][j];
                }
            }
            return _bits;
        }

        int nonZero(const BitsType _bits) {
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    if (_bits[i][j] != 0) {
                        return 1;
                    }
                }
            }
            return 0;
        }

        // shifts the whole bitboard in one direction
        // accounts for overeflow between constituent parts
        BitsType shiftOne(BitsType _bits, const types::Direction dir) {
            switch (dir) {
                case types::Direction::NORTHEAST:
                    _bits = shiftOne(shiftOne(_bits, types::Direction::EAST), types::Direction::NORTH);
                    break;
                case types::Direction::SOUTHEAST:
                    _bits = shiftOne(shiftOne(_bits, types::Direction::EAST), types::Direction::SOUTH);
                    break;
                case types::Direction::SOUTHWEST:
                    _bits = shiftOne(shiftOne(_bits, types::Direction::WEST), types::Direction::SOUTH);
                    break;
                case types::Direction::NORTHWEST:
                    _bits = shiftOne(shiftOne(_bits, types::Direction::WEST), types::Direction::NORTH);
                    break;
                default:
                    BitsType overflows = boardDefaults::zeroed;
                    const int dirIndex = helper::indexFromDirection(dir);

                    for (int rowIndex = 0; rowIndex < ROWS; rowIndex++) {
                        for (int colIndex = 0; colIndex < COLS; colIndex++) {
                            const std::uint16_t subBoard = _bits[rowIndex][colIndex];
                            // calculate overflows
                            const std::uint16_t vert = lShift(subBoard & vertMasks[dirIndex], vertShifts[dirIndex]);
                            const std::uint16_t hori = lShift(subBoard & horiMasks[dirIndex], horiShifts[dirIndex]);
                            const int newRowIndex = rowIndex + vertIndex[dirIndex];
                            const int newColIndex = colIndex + horiIndex[dirIndex];

                            // cache overflows
                            if (newRowIndex >= 0 && newRowIndex < ROWS) {
                                overflows[newRowIndex][colIndex] |= vert;
                            }
                            if (newColIndex >= 0 && newColIndex < COLS) {
                                overflows[rowIndex][newColIndex] |= hori;
                            }
                            // change original subboard 
                            _bits[rowIndex][colIndex] = lShift(subBoard, shifts[dirIndex]) & excludeMasks[dirIndex];
                        }
                    }

                    // apply overflows at end
                    // this is done at the end to stop the above shifts from being overwritten
                    for (int rowIndex = 0; rowIndex < ROWS; rowIndex++) {
                        for (int colIndex = 0; colIndex < COLS; colIndex++) {
                            _bits[rowIndex][colIndex] |= overflows[rowIndex][colIndex];
                        }
                    }
            }
            return _bits;
        }

        // like shiftone except it adds an or operation after the shift with the origina word
        BitsType extend(BitsType _bits, const types::Direction dir) {
            return bitsApplyOr(shiftOne(_bits, dir), _bits);
        }

        BitsType bitsTernOr(BitsType _bits1, const BitsType _bits2, const BitsType _bits3) {
            _bits1 = bitsApplyOr(_bits1, _bits2);
            _bits1 = bitsApplyOr(_bits1, _bits3);
            return _bits1;
        }

        // removes attacks on friendly pieces
        // removes attacks on out of bounds spaces
        // should be enough for pseudolegality
        BitsType sanitiseAttacks(const BitsType _bits, const BitsType friendlies) {
            // remove out of bounds
            const BitsType inBounds = bitsApplyAnd(boardDefaults::inBoard, _bits);

            // remove attacks on friendlies
            return bitsApplyAnd(inBounds, bitsApplyNot(friendlies));
        }

        /*
         * gets the ray shift in a direction
         * @params _bits the bits to shift
         * @params stopCase the stop case telling the func when to stop
         * @params dir the direction to shift in 
         * @returns the ray bits
         */
        BitsType rayShift(const BitsType _bits, const BitsType stopCase, const types::Direction dir) {
            boundsCheck(_bits);
            // shift first
            BitsType working = shiftOne(_bits, dir);

            // check if wever reached the stopCase
            while (!nonZero(bitsApplyAnd(working, stopCase))) {
                working = extend(working, dir);
            }
            return working;
        }

        BitsType pawnQuietShift(const BitsType _bits, const types::PieceColour col, const int isStarting, const BitsType blockers) {
            boundsCheck(_bits);
            const types::Direction forward = helper::getUp(col);
            BitsType res = _bits;
            BitsType shifted = shiftOne(_bits, forward);
            if (nonZero(bitsApplyAnd(shifted, blockers))) {
                return boardDefaults::zeroed;
            }
            res = shifted;
            if (isStarting) {
                shifted = extend(shifted, forward);
                if (nonZero(bitsApplyAnd(shifted, blockers))) {
                    return res;
                }
                res = shifted;
            }
            return res;
        }

        BitsType pawnLoudShift(const BitsType _bits, const types::PieceColour col, const BitsType enemies) {
            boundsCheck(_bits);
            const types::Direction upLeft = helper::getUpLeft(col);
            const types::Direction upRight = helper::getUpRight(col);

            BitsType ret = boardDefaults::zeroed;
            const BitsType shiftUpLeft = shiftOne(_bits, upLeft);
            if (nonZero(bitsApplyAnd(shiftUpLeft, enemies))) {
                ret = shiftUpLeft;
            }

            const BitsType shiftUpRight = shiftOne(_bits, upRight);
            if (nonZero(bitsApplyAnd(shiftUpRight, enemies))) {
                ret = bitsApplyOr(ret, shiftUpRight);
            }
            return ret;
        }

        BitsType pawnShift(const BitsType _bits, const types::PieceColour col, const BitsType blockers, const BitsType enemies, const BitsType friendlies) {
            boundsCheck(_bits);
            int isStarting = 0;
            switch (col) {
                case types::PieceColour::RED:
                    isStarting = nonZero(bitsApplyAnd(_bits, boardDefaults::redPawns));
                    break;
                case types::PieceColour::BLUE:
                    isStarting = nonZero(bitsApplyAnd(_bits, boardDefaults::bluePawns));
                    break;
                case types::PieceColour::GREEN:
                    isStarting = nonZero(bitsApplyAnd(_bits, boardDefaults::greenPawns));
                    break;
                case types::PieceColour::YELLOW:
                    isStarting = nonZero(bitsApplyAnd(_bits, boardDefaults::yellowPawns));
                    break;
                default:
                    break;
            }

            return sanitiseAttacks(bitsApplyOr(pawnQuietShift(_bits, col, isStarting, blockers), pawnLoudShift(_bits, col, enemies)), friendlies);
        }

        BitsType rookShift(const BitsType _bits, const BitsType blockers, const BitsType friendlies) {
            boundsCheck(_bits);
            const std::array<types::Direction, 4> dirs = {helper::NORTH, helper::EAST, helper::SOUTH, helper::WEST};
            BitsType result = boardDefaults::zeroed;

            BitsType stopCase = bitsApplyOr(boardDefaults::inaccessible, blockers);
            for (types::Direction dir : dirs) {
                result = bitsApplyOr(result, rayShift(_bits, stopCase, dir));
            }
            return sanitiseAttacks(result, friendlies);
        }

        BitsType bishopShift(const BitsType _bits, const BitsType blockers, const BitsType friendlies) {
            boundsCheck(_bits);
            const std::array<types::Direction, 4> dirs = {types::Direction::NORTHEAST, types::Direction::SOUTHEAST, types::Direction::SOUTHWEST, types::Direction::NORTHWEST};
            BitsType result = boardDefaults::zeroed;

            BitsType stopCase = bitsApplyOr(boardDefaults::inaccessible, blockers);
            for (types::Direction dir : dirs) {
                result = bitsApplyOr(result, rayShift(_bits, stopCase, dir));
            }
            return sanitiseAttacks(result, friendlies);
        }

        BitsType knightShift(const BitsType _bits, const BitsType friendlies) {
            boundsCheck(_bits);
            const int dirsLength = 8;
            const std::array<const std::array<types::Direction, 3>, dirsLength> dirs = {{
                {{helper::NORTH, helper::NORTH, helper::EAST}},
                    {{helper::NORTH, helper::NORTH, helper::WEST}},
                    {{helper::SOUTH, helper::SOUTH, helper::EAST}},
                    {{helper::SOUTH, helper::SOUTH, helper::WEST}},
                    {{helper::EAST, helper::EAST, helper::NORTH}},
                    {{helper::EAST, helper::EAST, helper::SOUTH}},
                    {{helper::WEST, helper::WEST, helper::NORTH}},
                    {{helper::WEST, helper::WEST, helper::SOUTH}}
            }};

            BitsType results = boardDefaults::zeroed;

            // apply the 3 shifts
            for (int i = 0; i < dirsLength; i++) {
                BitsType shifted = shiftOne(_bits, dirs[i][0]);
                shifted = shiftOne(shifted, dirs[i][1]);
                shifted = shiftOne(shifted, dirs[i][2]);
                results = bitsApplyOr(results, shifted);
            }

            return sanitiseAttacks(results, friendlies);
        }

        /*
         * shift generateed by assuming it can move in all directions
         * checks against blockers for castling
         */
        BitsType kingShift(
                const BitsType _bits, const std::pair<int, int> hasCastlingRights, const types::PieceColour colour, 
                const BitsType blockers, const BitsType unMovedRooks, const BitsType friendlies
                ) {
            BitsType ret = boardDefaults::zeroed;
            for (types::Direction dir : helper::DIRECTIONS) {
                ret = sanitiseAttacks(bitsApplyOr(ret, shiftOne(_bits, dir)), friendlies);
            }
            if (hasCastlingRights.first) {
                const types::Direction left = helper::getLeft(colour);

                const BitsType stopCase = bitsApplyOr(blockers, boardDefaults::inaccessible);
                BitsType rays = rayShift(_bits, stopCase, left);

                ret = bitsApplyOr(ret, bitsApplyAnd(unMovedRooks, rays));
            }
            if (hasCastlingRights.second) {
                const types::Direction right = helper::getRight(colour);

                const BitsType stopCase = bitsApplyOr(blockers, boardDefaults::inaccessible);
                BitsType rays = rayShift(_bits, stopCase, right);

                ret = bitsApplyOr(ret, bitsApplyAnd(unMovedRooks, rays));
            }
            return ret;
        }

        // gets the ray shift for a singular attacker on the board
        BitsType queenShift(const BitsType _bits, const BitsType blockers, const BitsType friendlies) {
            return bitsApplyOr(rookShift(_bits, blockers, friendlies), bishopShift(_bits, blockers, friendlies));
        }

        std::uint16_t bitScan(std::uint16_t x) {
            return x & -x;
        }

        int popCount(std::uint16_t x) {
            int i = 0;
            while(x) {
                i++;
                x &= x - 1;
            }
            return i;
        }

        // rotates 90 degrees anticlockwise about center
        void rotate(BitsType bits) {
            BitsType tmp = bits;
            // transpose and reverse 
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    bits[ROWS - j - 1][i] = rotateWord(tmp[i][j], 1);
                }
            }
        }
    }

    void boundsCheck(BitsType _bits) {
        assert(!nonZero(bitsApplyAnd(_bits, boardDefaults::inaccessible)));
    }

    int boolBoundsCheck(BitsType _bits) {
        return nonZero(bitsApplyAnd(_bits, boardDefaults::inaccessible));
    }

    // should represent a set of pieces on the board according to a colour
    class Bitboard {
        BitsType bits;
        public:
            Bitboard(std::pair<int, int> pCoords)
            : bits(boardDefaults::zeroed)
            {
                int outerRow = pCoords.first / 4;
                int innerRow = pCoords.first % 4;
                int outerCol = pCoords.second / 4;
                int innerCol = pCoords.second % 4;
                std::uint16_t word = 1 << innerCol;
                word = word << ((3 - innerRow) * 4);
                bits[outerRow][outerCol] = word;
            }

            Bitboard(BitsType _bits = {0, 0, 0, 0}) : bits(_bits) {}
            // bitboard is padded to include empty rows follwoing a 16*18 pattern
            // uses 16 8 20 implementation due to lack of precisely sized words
            // (4*5) * 16 = 16 x 20 board = 320 -> alot less overhead
            // use little endian for board layout
            // so a1 = bits[0][0] & 2^9
            // a5 = bits[1][0] & 2^1

            bool operator==(const Bitboard &other) const {
                return bits == other.bits;
            }
            
            bool operator!=(const Bitboard &other) const {
                return !(*this == other);
            }

            // uses little endianness for everythin
            void print(std::string_view header = "BitBoard") {
                const int iBoardDimensions = 4;
                std::array<std::stringstream, ROWS * iBoardDimensions> tmp;

                for (int oRowIndex = 0; oRowIndex < ROWS; oRowIndex++) {
                    for (int oColIndex = 0; oColIndex < COLS; oColIndex++) {
                        const std::uint16_t subBoard = bits[oRowIndex][oColIndex];
                        for (int i = 0; i < iBoardDimensions; i++) {
                            const std::uint16_t rowBits = 0xF000 & (subBoard << (iBoardDimensions * i));
                            for (int j = 0; j < iBoardDimensions; j++) {
                                // get each least significant bit in the row and add to print object
                                const int lsb = 0x1000 & (rowBits >> j);
                                tmp[oRowIndex * iBoardDimensions + i] << (lsb ? '1' : '0');
                                if ((oColIndex == 0 && tmp[oRowIndex * iBoardDimensions + i].str().length() == 1)
                                        || (oColIndex == COLS - 1 && tmp[oRowIndex * iBoardDimensions + i].str().length() == 16)) {
                                    tmp[oRowIndex * iBoardDimensions + i] << '|';
                                }
                            }
                        }
                    }
                }


                using namespace std::string_view_literals;
                std::cout << "    "sv;
                std::cout << header;
                std::cout << "    \n"sv;
                for (unsigned int i = 0; i < tmp.size(); i++) {
                    std::cout << tmp[i].str() << '\n';
                    if (i == 0 || i == tmp.size() - 2) {
                        std::cout << "-|--------------|-\n"sv;
                    }
                }
                std::cout << "            \n"sv;
                std::cout.flush();
            }

            coords bitIndex() const {
                coords ret = {0, 0, 3, 3};
                std::uint16_t bit = 0;
                for (int i = 0; i < ROWS; i++) {
                    for (int j = 0; j < COLS; j++) {
                        if (bits[i][j] == 0 ) {
                            continue;
                        }
                        bit = bitScan(bits[i][j]);
                        ret.outerY = i;
                        ret.outerX = j;
                        goto inner;
                    }
                }
                inner:
                assert(bit != 0);
                while (bit > 0x0008) {
                    ret.innerY--;
                    bit = bit >> 4;
                }
                switch (bit) {
                    case 0x0001:
                        ret.innerX = 0;
                        break;
                    case 0x0002:
                        ret.innerX = 1;
                        break;
                    case 0x0004:
                        ret.innerX = 2;
                        break;
                    case 0x0008:
                        ret.innerX = 3;
                        break;
                    default:
                        // how did we get here?
                        assert(false);
                }
                return ret;
            }

            /*
             * pops the first bit 
             * @returns a unary bitboard with the popped bit
             */
            Bitboard popBit() {
                BitsType data = boardDefaults::zeroed;
                for (int i = 0; i < ROWS; i++) {
                    for (int j = 0; j < COLS; j++) {
                        if (bits[i][j] != 0) {
                            std::uint16_t bit = bitScan(bits[i][j]);
                            // add to output
                            data[i][j] |= bit;
                            // remove from our board
                            bits[i][j] = bits[i][j] & ~bit;
                            return Bitboard(data);
                        }
                    }
                }
                return Bitboard(data);
            }

            int empty() const {
                return !nonZero(bits);
            }

            int unary() const {
                const BitsType tmp = bits;
                int sum = 0;
                for (int i = 0; i < ROWS; i++) {
                    for (int j = 0; j < COLS; j++) {
                        sum += popCount(tmp[i][j]);
                        if (sum > 1) {
                            return 0;
                        }
                    }
                }
                return sum == 1;
            }
            
            int count() const {
                int count = 0;
                for (int i = 0; i < ROWS; i++) {
                    for (int j = 0; j < COLS; j++) {
                        std::uint16_t sub = bits[i][j];
                        while (sub) {
                            count++;
                            sub &= sub - 1;
                        }
                    }
                }
                return count;
            }

            Bitboard subtract(const Bitboard board) const {
                return intersect(board.inverse());
            }

            Bitboard inverse() const {
                return Bitboard(bitsApplyNot(bits));
            }

            /*
             * combines 2 bitboard as a union
             *
             */
            Bitboard combine(const Bitboard board) const {
                return Bitboard(bitsApplyOr(board.bits, bits));
            }

            Bitboard intersect(const Bitboard board) const {
                return Bitboard(bitsApplyAnd(bits, board.bits));
            }

            Bitboard genRookShift(Bitboard blockers, const Bitboard friendlies) const {
                return Bitboard(rookShift(bits, blockers.bits, friendlies.bits));
            }

            Bitboard genBishopShift(Bitboard blockers, const Bitboard friendlies) const {
                return Bitboard(bishopShift(bits, blockers.bits, friendlies.bits));
            }

            Bitboard genKnightShift(const Bitboard friendlies) const {
                return Bitboard(knightShift(bits, friendlies.bits));
            }

            Bitboard genQueenShift(Bitboard blockers, const Bitboard friendlies) const {
                return Bitboard(queenShift(bits, blockers.bits, friendlies.bits));
            }

            Bitboard genKingShift(
                    const std::pair<int, int> hasCastlingRights, 
                    const types::PieceColour colour, const Bitboard blockers, 
                    const Bitboard unMovedRooks, const Bitboard friendlies) const {
                return Bitboard(kingShift(bits, hasCastlingRights, colour, blockers.bits, unMovedRooks.bits, friendlies.bits));
            }

            Bitboard genPawnShift(types::PieceColour col, Bitboard blockers, Bitboard enemies, const Bitboard friendlies) const {
                return Bitboard(pawnShift(bits, col, blockers.bits, enemies.bits, friendlies.bits));
            }
            
            // translates a bitboard to a 16* 16 index
            std::pair<int, int> to16RC() const {
                assert(unary());
                bitboard::coords c = bitIndex();
                int x = (c.innerX + c.outerX * 4);
                int y = (c.innerY + c.outerY * 4);

                return std::pair(x,y);
            }

            void boundsCheck() const {
                ::bitboard::boundsCheck(bits);
            }

            int boolBoundsCheck() const __attribute__((noinline)) {
                return ::bitboard::boolBoundsCheck(bits);
            }
    };

    Bitboard nullBoard = Bitboard(boardDefaults::nullBoard);
}

#endif
