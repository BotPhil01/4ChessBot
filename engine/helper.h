#include "bitboard.h"
#include"types.h"
#include<cassert>
#include <string_view>
#include<vector>
#include<utility>
#include<memory>
#include<array>

#ifndef HELPER_H
#define HELPER_H

// helper functions and constants
namespace helper
{
    
    constexpr types::PieceColour RED = types::PieceColour::RED;
    constexpr types::PieceColour BLUE = types::PieceColour::BLUE;
    constexpr types::PieceColour YELLOW = types::PieceColour::YELLOW;
    constexpr types::PieceColour GREEN = types::PieceColour::GREEN;
    constexpr types::Direction NORTH = types::Direction::NORTH;
    constexpr types::Direction EAST = types::Direction::EAST;
    constexpr types::Direction SOUTH = types::Direction::SOUTH;
    constexpr types::Direction WEST = types::Direction::WEST;
    
    constexpr  types::Direction REDUP = types::Direction::NORTH;
    constexpr  types::Direction REDDOWN = types::Direction::SOUTH;
    constexpr  types::Direction REDLEFT = types::Direction::WEST;
    constexpr  types::Direction REDRIGHT = types::Direction::EAST;
    constexpr  types::Direction REDUPRIGHT = types::Direction::NORTHEAST;
    constexpr  types::Direction REDUPLEFT = types::Direction::NORTHWEST;
    constexpr  types::Direction REDDOWNRIGHT = types::Direction::SOUTHEAST;
    constexpr  types::Direction REDDOWNLEFT = types::Direction::SOUTHWEST;

    constexpr  types::Direction BLUEUP = types::Direction::EAST;
    constexpr  types::Direction BLUEDOWN = types::Direction::WEST;
    constexpr  types::Direction BLUELEFT = types::Direction::NORTH;
    constexpr  types::Direction BLUERIGHT = types::Direction::SOUTH;
    constexpr  types::Direction BLUEUPRIGHT = types::Direction::SOUTHEAST;
    constexpr  types::Direction BLUEUPLEFT = types::Direction::NORTHEAST;
    constexpr  types::Direction BLUEDOWNRIGHT = types::Direction::SOUTHWEST;
    constexpr  types::Direction BLUEDOWNLEFT = types::Direction::NORTHWEST;

    constexpr types::Direction YELLOWUP = types::Direction::SOUTH;
    constexpr types::Direction YELLOWDOWN = types::Direction::NORTH;
    constexpr types::Direction YELLOWLEFT = types::Direction::EAST;
    constexpr types::Direction YELLOWRIGHT = types::Direction::WEST;
    constexpr types::Direction YELLOWUPRIGHT = types::Direction::SOUTHWEST;
    constexpr types::Direction YELLOWUPLEFT = types::Direction::SOUTHEAST;
    constexpr types::Direction YELLOWDOWNRIGHT = types::Direction::NORTHWEST;
    constexpr types::Direction YELLOWDOWNLEFT = types::Direction::NORTHEAST;

    constexpr types::Direction GREENUP = types::Direction::WEST;
    constexpr types::Direction GREENDOWN = types::Direction::EAST;
    constexpr types::Direction GREENRIGHT = types::Direction::NORTH;
    constexpr types::Direction GREENLEFT = types::Direction::SOUTH;
    constexpr types::Direction GREENUPRIGHT = types::Direction::NORTHWEST;
    constexpr types::Direction GREENUPLEFT = types::Direction::SOUTHWEST;
    constexpr types::Direction GREENDOWNRIGHT = types::Direction::NORTHEAST;
    constexpr types::Direction GREENDOWNLEFT = types::Direction::SOUTHEAST;
    
    constexpr std::array<const types::Direction, 8UL> DIRECTIONS = {
        types::Direction::NORTH, types::Direction::NORTHEAST, 
        types::Direction::EAST, types::Direction::SOUTHEAST, 
        types::Direction::SOUTH, types::Direction::SOUTHWEST, 
        types::Direction::WEST, types::Direction::NORTHWEST};

    constexpr unsigned char BOARDLENGTH = 196;
    constexpr unsigned char PADDEDCOLS = 16;
    constexpr unsigned char PADDEDROWS = 18;
    
    constexpr std::array<const types::PieceColour, 4UL> playableColours {RED, BLUE, YELLOW, GREEN};
    constexpr std::array<const types::PieceType, 6UL> playablePieces {types::PieceType::PAWN, types::PieceType::ROOK, types::PieceType::KNIGHT, types::PieceType::BISHOP, types::PieceType::QUEEN, types::PieceType::KING};

    constexpr std::size_t NPIECES = 6;
    constexpr std::size_t NCOLOURS = 4;

    // constexpr std::array<types::boardIndex, 36> cornerIndices = {
    //     33, 34, 35, 49, 50, 51, 65, 66, 67,           // sw
    //     44, 45, 46, 60, 61, 62, 76, 77, 78,           // se
    //     209, 210, 211, 225, 226, 227, 241, 242, 243,  // nw
    //     220, 221, 222, 236, 237, 238, 252, 253, 254}; // ne

    constexpr types::PieceColour initailTurn = types::PieceColour::RED;

    constexpr const int indexFromDirection(types::Direction dir) {
        switch(dir) {
            case types::Direction::NORTH:
                return 0;
            case types::Direction::NORTHEAST:
                return 1;
            case types::Direction::EAST:
                return 2;
            case types::Direction::SOUTHEAST:
                return 3;
            case types::Direction::SOUTH:
                return 4;
            case types::Direction::SOUTHWEST:
                return 5;
            case types::Direction::WEST:
                return 6;
            case types::Direction::NORTHWEST:
                return 7;
            default:
                return -1;
        }
        return -1;
    }

    constexpr const int indexFromType(types::PieceType t) {
        assert(t != types::PieceType::BLOCK && t != types::PieceType::EMPTY);
        for (unsigned int i = 0; i < NPIECES; ++i) {
            if (playablePieces[i] == t) {
                return i;
            }
        }
        return -1;
    }

    constexpr int indexFromColour(types::PieceColour const c) {
        switch (c){
            case RED:
                return 0;
            case BLUE:
                return 1;
            case YELLOW:
                return 2;
            case GREEN:
                return 3;
            default:
                return -1;
        }
        return -1;
    }

    constexpr types::Direction getLeft(types::PieceColour const c)
    {
        switch (c)
        {
        case types::PieceColour::RED:
            return REDLEFT;
        case types::PieceColour::GREEN:
            return GREENLEFT;
        case types::PieceColour::BLUE:
            return BLUELEFT;
        case types::PieceColour::YELLOW:
            return YELLOWLEFT;
        default:
            return REDLEFT;
        }
    }

    constexpr types::Direction getRight(types::PieceColour const c)
    {
        switch (c)
        {
        case types::PieceColour::RED:
            return REDRIGHT;
        case types::PieceColour::GREEN:
            return GREENRIGHT;
        case types::PieceColour::BLUE:
            return BLUERIGHT;
        case types::PieceColour::YELLOW:
            return YELLOWRIGHT;
        default:
            return REDRIGHT;
        }
    }

    constexpr types::Direction getUp(types::PieceColour const c)
    {
        switch (c)
        {
        case types::PieceColour::RED:
            return REDUP;
        case types::PieceColour::GREEN:
            return GREENUP;
        case types::PieceColour::BLUE:
            return BLUEUP;
        case types::PieceColour::YELLOW:
            return YELLOWUP;
        default:
            return REDUP;
        }
    }

    constexpr types::Direction getDown(types::PieceColour const c)
    {
        switch (c)
        {
        case types::PieceColour::RED:
            return REDDOWN;
        case types::PieceColour::GREEN:
            return GREENDOWN;
        case types::PieceColour::BLUE:
            return BLUEDOWN;
        case types::PieceColour::YELLOW:
            return YELLOWDOWN;
        default:
            return REDDOWN;
        }
    }

    constexpr types::Direction getUpRight(types::PieceColour const c)
    {
        switch (c)
        {
        case types::PieceColour::RED:
            return REDUPRIGHT;
        case types::PieceColour::GREEN:
            return GREENUPRIGHT;
        case types::PieceColour::BLUE:
            return BLUEUPRIGHT;
        case types::PieceColour::YELLOW:
            return YELLOWUPRIGHT;
        default:
            return REDUPRIGHT;
        }
    }

    constexpr types::Direction getUpLeft(types::PieceColour const c)
    {
        switch (c)
        {
        case types::PieceColour::RED:
            return REDUPLEFT;
        case types::PieceColour::GREEN:
            return GREENUPLEFT;
        case types::PieceColour::BLUE:
            return BLUEUPLEFT;
        case types::PieceColour::YELLOW:
            return YELLOWUPLEFT;
        default:
            return REDUPLEFT;
        }
    }
    
    // constexpr bool indexInCorners(types::boardIndex i) {
    //     for (auto index : cornerIndices) {
    //         if (index == i) {
    //             return true;
    //         }
    //     }
    //     return false;
    // }
    
    // constexpr bool indexOnBoard(types::boardIndex const i) {
    //     int rem = i % 16;
    //     return (i > 35 && i < 252 && rem != 0 && rem != 15 && !indexInCorners(i));
    // }

    // constexpr bool isRedStart(types::boardIndex const i)
    // {
    //     return (i > 35 && i < 44) || (i > 51 && i < 60);
    // }

    // constexpr bool isYellowStart(types::boardIndex const i)
    // {
    //     return (i > 227 && i < 236) || (i > 243 && i < 252);
    // }

    // constexpr bool isBlueStart(types::boardIndex const i)
    // {
    //     short rem = i % 16;
    //     return (rem == 1 || rem == 2) && i < 195 && i > 80;
    // }

    // constexpr bool isGreenStart(types::boardIndex const i)
    // {
    //     short rem = i % 16;
    //     return (rem == 13 || rem == 14) && i > 92 && i < 207;
    // }

    // constexpr bool isPawnStart(types::boardIndex const i)
    // {
    //     if ((i > 51 && i < 60) || (i > 227 && i < 236))
    //     { // red and yellow
    //         return true;
    //     }
    //     short rem = i % 16;
    //     return (i > 81 && i < 207 && (rem == 13 || rem == 2));
    // }

    // constexpr bool isRookStart(types::boardIndex const i)
    // {
    //     return i == 36 || i == 43 || i == 81 || i == 94 || i == 193 || i == 206 || i == 244 || i == 251;
    // }

    // constexpr bool isKnightStart(types::boardIndex const i)
    // {
    //     return i == 37 || i == 42 || i == 97 || i == 110 || i == 177 || i == 190 || i == 245 || i == 250;
    // }

    // constexpr bool isBishopStart(types::boardIndex const i)
    // {
    //     return i == 38 || i == 41 || i == 113 || i == 126 || i == 161 || i == 174 || i == 246 || i == 249;
    // }

    // constexpr bool isQueenStart(types::boardIndex const i)
    // {
    //     return i == 39 || i == 129 || i == 158 || i == 248;
    // }

    // constexpr bool isKingStart(types::boardIndex const i)
    // {
    //     return i == 40 || i == 145 || i == 142 || i == 247;
    // }

    // constexpr bool isEmptyStart(types::boardIndex const i)
    // {
    //     short tmp = i % 16;
    //     bool in14Square = (i > 32 && i < 255 && tmp != 15 && tmp != 0);
    //     bool notInCorners = true;
    //     
    //     for (unsigned int j = 0; j < cornerIndices.size() ; ++j) {
    //         types::boardIndex tmpIndex = cornerIndices[j];
    //         if (i == tmpIndex)
    //         {
    //             notInCorners = false;
    //             break;
    //         }
    //     }
    //     return notInCorners && in14Square;
    // }

    // constexpr types::Square generateSquare(short i) {
    //     // assign colour
    //     types::PieceColour pc = types::PieceColour ::NONE;
    //     pc = isRedStart(i) ? types::PieceColour::RED : isBlueStart(i) ? types::PieceColour::BLUE
    //                                         : isYellowStart(i) ? types::PieceColour::YELLOW
    //                                         : isGreenStart(i)  ? types::PieceColour::GREEN
    //                                                            : types::PieceColour::NONE;
    //     // assign type
    //     types::PieceType pt = types::PieceType::BLOCK;
    //     pt = isEmptyStart(i) ? types::PieceType::EMPTY : pt; // empty
    //     pt = isRookStart(i) ? types::PieceType::ROOK : isKnightStart(i) ? types::PieceType::KNIGHT
    //                                         : isBishopStart(i)   ? types::PieceType::BISHOP
    //                                                              : pt;
    //     pt = isPawnStart(i) ? types::PieceType::PAWN : isKingStart(i) ? types::PieceType::KING
    //                                         : isQueenStart(i)  ? types::PieceType::QUEEN
    //                                                            : pt;
    //     return types::Square(pt != types::PieceType::BLOCK, pt, pc);
    // };

    // concat uses this for vectors
    template <typename T, size_t size1, size_t size2>
    constexpr void concat(std::array<T, size1> &v1, std::array<T, size2> v2) {
        unsigned int v1size = 0;
        for (unsigned int i = 0; i < v1.size(); ++i) {
            if (v1[i] == 0) {
                break;
            }
            v1size = i;
        }

        for (unsigned int i = 0; i < v2.size(); ++i) {
            if (v2[i] == 0) {
                break;
            }
            v1[i + v1size] = v2[i];
        }
    }

    template <typename T, size_t size>
    constexpr std::array<T, size> layer(std::array<T, size> v1, std::array<T, size> v2) {
        for (unsigned int i = 0; i < size; i++) {
            v1[i] += v2[i];
        }
        return v1;
    }

    template<typename T, size_t size>
    constexpr void multiplyValues(std::array<T, size> &v, T f) {
        for (unsigned char i = 0; i < size; ++i) {
            v[i] *= f;
        }
    }

    

    constexpr char typeToChar(types::PieceType t)
    {
        switch (t) {
        case types::PieceType::PAWN:
            return 'P';
        case types::PieceType::ROOK:
            return 'R';
        case types::PieceType::KNIGHT:
            return 'N';
        case types::PieceType::BISHOP:
            return 'B';
        case types::PieceType::QUEEN:
            return 'Q';
        case types::PieceType::KING:
            return 'K';
        case types::PieceType::EMPTY:
            return 'E';
        default:
            return '#';
        };
    };

    constexpr char colourToChar(types::PieceColour t)
    {
        using pc = types::PieceColour;
        switch (t) {
        case pc::RED:
            return 'R';
        case pc::GREEN:
            return 'G';
        case pc::BLUE:
            return 'B';
        case pc::YELLOW:
            return 'Y';
        case pc::NONE:
            return ' ';
        default:
            return ' ';
        };
    }


    
    constexpr unsigned int getColourIndex(types::PieceColour c) {
        assert(c != types::PieceColour::NONE);
        switch (c) {
            case types::PieceColour::RED:
                return 0;
            case types::PieceColour::BLUE:
                return 1;
            case types::PieceColour::YELLOW:
                return 2;
            case types::PieceColour::GREEN:
                return 3;
            default:
                return -1;
        }
        return -1;
    }

    constexpr types::PieceColour getColourFromIndex(char i) {
        assert(i >= 0 && i < 4);
        switch (i) {
            case 0:
                return types::PieceColour::RED;
            case 1:
                return types::PieceColour::BLUE;
            case 2:
                return types::PieceColour::YELLOW;
            case 3:
                return types::PieceColour::GREEN;
            default:
                return types::PieceColour::NONE;
        }
        return types::PieceColour::NONE;
    }
    // // assumes there is a valid non diagonal straight path towards tgt from src
    // constexpr types::Direction getDirection(types::boardIndex src, types::boardIndex tgt) {
    //     short diff = tgt - src;
    //     if (diff > 0) {
    //         if (diff > PADDEDCOLS) {
    //             return types::Direction::NORTH;
    //         }
    //         return types::Direction::EAST;
    //     }
    //     if (-diff > PADDEDCOLS) {
    //         return types::Direction::SOUTH;
    //     }
    //     return types::Direction::WEST;
    // }

    // constexpr types::boardIndex shiftOne(const types::boardIndex i, const types::Direction d) {
    //     switch (d) {
    //         case types::Direction::NORTH: 
    //             return i + 16;
    //         case types::Direction::NORTHEAST: 
    //             return i + 17;
    //         case types::Direction::EAST:
    //             return i + 1;
    //         case types::Direction::SOUTHEAST:
    //             return i - 15;
    //         case types::Direction::SOUTH:
    //             return i - 16;
    //         case types::Direction::SOUTHWEST:
    //             return i - 17;
    //         case types::Direction::WEST:
    //             return i - 1;
    //         case types::Direction::NORTHWEST:
    //             return i + 15;
    //         default:
    //             return i;
    //     };
    // }

    // // returns types::boardIndex from 16 * 18  coordinates
    // constexpr types::boardIndex toIndex( unsigned char c, unsigned char r) { 
    //     return (r * 16 + c);
    // }


    constexpr unsigned int asc2PadCol(char x) {
        // lowercase the char
        x = x < (int) 'a' ? x + 32: x;
        // geet index
        x = x - 'a';
        // add padding
        return x + 1;
    }
    
    // passes in '1' -> '14'
    constexpr unsigned int asc2PadRow(std::string_view str) {
        const size_t size = str.length();
        int first = 0;
        int second = 0;
        int index = -1;
        switch (size) {
            case 1:
                // convert to index 
                index = 14 - (str[0] - '0');
                // add padding
                return index + 1;
                break;
            case 2:
                first = str[0] - '0';
                second = str[1] - '0';
                index = 14 - (first * 10 - second);
                // add padding 
                return index + 1;
                break;
            default:
                assert(false);
        }
        return -1;
    }

    // // rotates a 14x14 index 90 degrees counter clockwise
    // constexpr types::boardIndex rotate90degrees(types::boardIndex i, unsigned char count) {
    //     std::pair<int, int> coords = {i % 14, (i - i % 14) / 14 };
    //     // find quadrant
    //     if (coords.second < 7) {
    //         // lower
    //         if (coords.first < 7) {
    //             // left
    //             coords.first -= 7;
    //             coords.second -= 7;
    //         } else {
    //             // right
    //             coords.first -= 6;
    //             coords.second -= 7;
    //         }
    //     } else {
    //         // upper
    //         if (coords.first < 7) {
    //             // left
    //             coords.first -= 6;
    //             coords.second -= 6;
    //         } else {
    //             // right
    //             coords.first -= 7;
    //             coords.second -= 6;
    //         }
    //     }

    //     while (count > 0) {   
    //         int tmp = coords.first;
    //         coords.first = -coords.second;
    //         coords.second = tmp;
    //         count--;
    //     }

    //     if (coords.first > 0) {
    //         coords.first += 6;
    //         if (coords.second > 0) {
    //             coords.second += 6;
    //         } else {
    //             coords.second += 7;
    //         }
    //     } else {
    //         coords.first += 7;
    //         if (coords.second > 0) {
    //             coords.second += 6;
    //         } else {
    //             coords.second += 7;
    //         }
    //     }

    //     i = coords.first + coords.second * 14;
    //     return i;
    // }
    // 
    // constexpr types::boardIndex to14BoardIndex(types::boardIndex i) {
    //     unsigned int rem = i % 16;
    //     assert(i > 32 && i < 255 && rem != 0 && rem != 15);
    //     i = (i -rem) / 16;
    //     return rem - 1 + ((i - 2) * 14);
    // }
};

#endif
