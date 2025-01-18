#include"types.h"
#include<cassert>
#include<vector>
#include<utility>
#ifndef HELPER_H
#define HELPER_H

// helper functions and constants
namespace helper
{
    
    const types::PieceColour RED = types::PieceColour::RED;
    const types::PieceColour BLUE = types::PieceColour::BLUE;
    const types::PieceColour YELLOW = types::PieceColour::YELLOW;
    const types::PieceColour GREEN = types::PieceColour::GREEN;
    const types::Direction NORTH = types::Direction::NORTH;
    const types::Direction EAST = types::Direction::EAST;
    const types::Direction SOUTH = types::Direction::SOUTH;
    const types::Direction WEST = types::Direction::WEST;
    
    const types::Direction REDUP = types::Direction::NORTH;
    const types::Direction REDDOWN = types::Direction::SOUTH;
    const types::Direction REDLEFT = types::Direction::WEST;
    const types::Direction REDRIGHT = types::Direction::EAST;
    const types::Direction REDUPRIGHT = types::Direction::NORTHEAST;
    const types::Direction REDUPLEFT = types::Direction::NORTHWEST;
    const types::Direction REDDOWNRIGHT = types::Direction::SOUTHEAST;
    const types::Direction REDDOWNLEFT = types::Direction::SOUTHWEST;

    const types::Direction BLUEUP = types::Direction::EAST;
    const types::Direction BLUEDOWN = types::Direction::WEST;
    const types::Direction BLUELEFT = types::Direction::NORTH;
    const types::Direction BLUERIGHT = types::Direction::SOUTH;
    const types::Direction BLUEUPRIGHT = types::Direction::SOUTHEAST;
    const types::Direction BLUEUPLEFT = types::Direction::NORTHEAST;
    const types::Direction BLUEDOWNRIGHT = types::Direction::SOUTHWEST;
    const types::Direction BLUEDOWNLEFT = types::Direction::NORTHWEST;

    const types::Direction YELLOWUP = types::Direction::SOUTH;
    const types::Direction YELLOWDOWN = types::Direction::NORTH;
    const types::Direction YELLOWLEFT = types::Direction::EAST;
    const types::Direction YELLOWRIGHT = types::Direction::WEST;
    const types::Direction YELLOWUPRIGHT = types::Direction::SOUTHWEST;
    const types::Direction YELLOWUPLEFT = types::Direction::SOUTHEAST;
    const types::Direction YELLOWDOWNRIGHT = types::Direction::NORTHWEST;
    const types::Direction YELLOWDOWNLEFT = types::Direction::NORTHEAST;

    const types::Direction GREENUP = types::Direction::WEST;
    const types::Direction GREENDOWN = types::Direction::EAST;
    const types::Direction GREENRIGHT = types::Direction::NORTH;
    const types::Direction GREENLEFT = types::Direction::SOUTH;
    const types::Direction GREENUPRIGHT = types::Direction::NORTHWEST;
    const types::Direction GREENUPLEFT = types::Direction::SOUTHWEST;
    const types::Direction GREENDOWNRIGHT = types::Direction::NORTHEAST;
    const types::Direction GREENDOWNLEFT = types::Direction::SOUTHEAST;
    
    const char PADDEDCOLS = 16;
    const char PADDEDROWS = 18;
    
    const std::vector<types::PieceColour> playableColours {RED, BLUE, YELLOW, GREEN};
    const std::vector<types::PieceType> playablePieces {types::PieceType::PAWN, types::PieceType::ROOK, types::PieceType::KNIGHT, types::PieceType::BISHOP, types::PieceType::QUEEN, types::PieceType::KING};

    const std::vector<types::boardIndex> cornerIndices = {
        33, 34, 35, 49, 50, 51, 65, 66, 67,           // sw
        44, 45, 46, 60, 61, 62, 76, 77, 78,           // se
        209, 210, 211, 225, 226, 227, 241, 242, 243,  // nw
        220, 221, 222, 236, 237, 238, 252, 253, 254}; // ne

    const types::PieceColour initailTurn = types::PieceColour::RED;



    const int indexFromType(types::PieceType t) {
        assert(t != types::PieceType::BLOCK && t != types::PieceType::EMPTY);
        for (unsigned int i = 0; i < playablePieces.size(); ++i) {
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
    
    const bool indexInCorners(types::boardIndex i) {
        for (auto index : cornerIndices) {
            if (index == i) {
                return true;
            }
        }
        return false;
    }
    
    constexpr bool indexOnBoard(types::boardIndex const i) {
        int rem = i % 16;
        return (i > 35 && i < 252 && rem != 0 && rem != 15 && !indexInCorners(i));
    }

    constexpr bool isRedStart(types::boardIndex const i)
    {
        return (i > 35 && i < 44) || (i > 51 && i < 60);
    }

    constexpr bool isYellowStart(types::boardIndex const i)
    {
        return (i > 227 && i < 236) || (i > 243 && i < 252);
    }

    constexpr bool isBlueStart(types::boardIndex const i)
    {
        short rem = i % 16;
        return (rem == 1 || rem == 2) && i < 195 && i > 80;
    }

    constexpr bool isGreenStart(types::boardIndex const i)
    {
        short rem = i % 16;
        return (rem == 13 || rem == 14) && i > 92 && i < 207;
    }

    constexpr bool isPawnStart(types::boardIndex const i)
    {
        if ((i > 51 && i < 60) || (i > 227 && i < 236))
        { // red and yellow
            return true;
        }
        short rem = i % 16;
        return (i > 81 && i < 207 && (rem == 13 || rem == 2));
    }

    constexpr bool isRookStart(types::boardIndex const i)
    {
        return i == 36 || i == 43 || i == 81 || i == 94 || i == 193 || i == 206 || i == 244 || i == 251;
    }

    constexpr bool isKnightStart(types::boardIndex const i)
    {
        return i == 37 || i == 42 || i == 97 || i == 110 || i == 177 || i == 190 || i == 245 || i == 250;
    }

    constexpr bool isBishopStart(types::boardIndex const i)
    {
        return i == 38 || i == 41 || i == 113 || i == 126 || i == 161 || i == 174 || i == 246 || i == 249;
    }

    constexpr bool isQueenStart(types::boardIndex const i)
    {
        return i == 39 || i == 129 || i == 158 || i == 248;
    }

    constexpr bool isKingStart(types::boardIndex const i)
    {
        return i == 40 || i == 145 || i == 142 || i == 247;
    }

    const bool isEmptyStart(types::boardIndex const i)
    {
        short tmp = i % 16;
        bool in14Square = (i > 32 && i < 255 && tmp != 15 && tmp != 0);
        bool notInCorners = true;
        
        for (unsigned int j = 0; j < cornerIndices.size() ; ++j) {
            types::boardIndex tmpIndex = cornerIndices[j];
            if (i == tmpIndex)
            {
                notInCorners = false;
                break;
            }
        }
        return notInCorners && in14Square;
    }

    types::Square generateSquare(short i)
    {
        // assign colour
        types::PieceColour pc = types::PieceColour ::NONE;
        pc = isRedStart(i) ? types::PieceColour::RED : isBlueStart(i) ? types::PieceColour::BLUE
                                            : isYellowStart(i) ? types::PieceColour::YELLOW
                                            : isGreenStart(i)  ? types::PieceColour::GREEN
                                                               : types::PieceColour::NONE;
        // assign type
        types::PieceType pt = types::PieceType::BLOCK;
        pt = isEmptyStart(i) ? types::PieceType::EMPTY : pt; // empty
        pt = isRookStart(i) ? types::PieceType::ROOK : isKnightStart(i) ? types::PieceType::KNIGHT
                                            : isBishopStart(i)   ? types::PieceType::BISHOP
                                                                 : pt;
        pt = isPawnStart(i) ? types::PieceType::PAWN : isKingStart(i) ? types::PieceType::KING
                                            : isQueenStart(i)  ? types::PieceType::QUEEN
                                                               : pt;
        return types::Square(pt != types::PieceType::BLOCK, pt, pc);
    };

    template <typename T>
    void concat(std::vector<T> &v1, std::vector<T> v2)
    {
        for (T i : v2)
        {
            v1.emplace_back(i);
        }
    }

    template <typename T>
    // layers vectors ontop of each other with + 
    // vectors must have same size
    std::vector<T> layer(std::vector<T> v1, std::vector<T> v2) {
        assert(v1.size() == v2.size());
        for (unsigned int i = 0; i < v1.size(); i++) {
            v1[i] += v2[i];
        }
        return v1;
    }

    template<typename T>
    void multiplyValues(std::vector<T> &v, T f) {
        for (unsigned int i = 0; i < v.size(); ++i) {
            v[i] *= f;
        }
    }


    constexpr std::string_view PieceTypeToString(types::PieceType t)
    {
        switch (t)
        {
        case types::PieceType::PAWN:
            return "PAWN";
        case types::PieceType::ROOK:
            return "ROOK";
        case types::PieceType::KNIGHT:
            return "KNIGHT";
        case types::PieceType::BISHOP:
            return "BISHOP";
        case types::PieceType::QUEEN:
            return "QUEEEN";
        case types::PieceType::KING:
            return "KING";
        case types::PieceType::EMPTY:
            return "EMPTY";
        default:
            return "BLOCK";
        };
    };

    constexpr std::string_view PieceColourToString(types::PieceColour t)
    {
        using pc = types::PieceColour;
        switch (t)
        {
        case pc::RED:
            return "RED";
        case pc::GREEN:
            return "GREEN";
        case pc::BLUE:
            return "BLUE";
        case pc::YELLOW:
            return "YELLOW";
        case pc::NONE:
            return "NONE";
        default:
            return "NOCOLOUR";
        };
    }

    // returns piece type as string view
    std::string_view pieceToStringView(types::Piece p)
    {
        return PieceTypeToString(p.type);
    }

    std::string moveToString(types::Move m)
    {
        std::string s = "";
        s.append(std::to_string(m.fromIndex()));
        s.append(std::to_string(m.toIndex()));
        return s;
    }

    // void printMoveVector(std::vector<Move> mvs) {
    //     std::cout << "Moves: ";
    //     for (Move m : mvs) {
    //         std::cout << moveToString(m) << ", ";
    //     }
    //     std::cout << "\n";
    // }

    // TODO DELETE AND REFACTOR
    unsigned int getColourIndex(types::PieceColour c) {
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

    types::PieceColour getColourFromIndex(char i) {
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
    // assumes there is a valid non diagonal straight path towards tgt from src
    types::Direction getDirection(types::boardIndex src, types::boardIndex tgt) {
        short diff = tgt - src;
        if (diff > 0) {
            if (diff > PADDEDCOLS) {
                return types::Direction::NORTH;
            }
            return types::Direction::EAST;
        }
        if (-diff > PADDEDCOLS) {
            return types::Direction::SOUTH;
        }
        return types::Direction::WEST;
    }

    // TODO DELETE AND REFACTOR
    // takes vector of length 4 a colour to has with and a value to place in vector
    void placeAtColourIndex(std::vector<float> * const ve, types::PieceColour c, float va) {
        (*ve)[getColourIndex(c)] = va;
    }

    types::boardIndex shiftOne(types::boardIndex i, types::Direction d) {
        switch (d) {
            case types::Direction::NORTH: 
                return i + 16;
            case types::Direction::NORTHEAST: 
                return i + 17;
            case types::Direction::EAST:
                return i + 1;
            case types::Direction::SOUTHEAST:
                return i - 15;
            case types::Direction::SOUTH:
                return i - 16;
            case types::Direction::SOUTHWEST:
                return i - 17;
            case types::Direction::WEST:
                return i - 1;
            case types::Direction::NORTHWEST:
                return i + 15;
            default:
                return i;
        };
    }

    // returns types::boardIndex from 16 * 18  coordinates
    types::boardIndex toIndex( unsigned char c, unsigned char r) { 
        return (r * 16 + c);
    }

    // translates a types::boardIndex to a padded row column pair
    std::pair<int, int> to16RC(types::boardIndex i) {
        int x = (i % 16);
        int y = (i - x) / 16;
        return std::pair(x,y);
    }

};

#endif