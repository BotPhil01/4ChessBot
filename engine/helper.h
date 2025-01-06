#include "types.h"
#include <vector>
using namespace types;
#ifndef HELPER_H
#define HELPER_H

// helper functions and constants
namespace helper
{
    const Piece EMPTYPIECE = Piece(PieceColour::NONE, PieceType::EMPTY);
    const Direction REDUP = Direction::NORTH;
    const Direction REDDOWN = Direction::SOUTH;
    const Direction REDLEFT = Direction::WEST;
    const Direction REDRIGHT = Direction::EAST;
    const Direction REDUPRIGHT = Direction::NORTHEAST;
    const Direction REDUPLEFT = Direction::NORTHWEST;
    const Direction REDDOWNRIGHT = Direction::SOUTHEAST;
    const Direction REDDOWNLEFT = Direction::SOUTHWEST;

    const Direction BLUEUP = Direction::EAST;
    const Direction BLUEDOWN = Direction::WEST;
    const Direction BLUELEFT = Direction::NORTH;
    const Direction BLUERIGHT = Direction::SOUTH;
    const Direction BLUEUPRIGHT = Direction::SOUTHEAST;
    const Direction BLUEUPLEFT = Direction::NORTHEAST;
    const Direction BLUEDOWNRIGHT = Direction::SOUTHWEST;
    const Direction BLUEDOWNLEFT = Direction::NORTHWEST;

    const Direction YELLOWUP = Direction::SOUTH;
    const Direction YELLOWDOWN = Direction::NORTH;
    const Direction YELLOWLEFT = Direction::EAST;
    const Direction YELLOWRIGHT = Direction::WEST;
    const Direction YELLOWUPRIGHT = Direction::SOUTHWEST;
    const Direction YELLOWUPLEFT = Direction::SOUTHEAST;
    const Direction YELLOWDOWNRIGHT = Direction::NORTHWEST;
    const Direction YELLOWDOWNLEFT = Direction::NORTHEAST;

    const Direction GREENUP = Direction::WEST;
    const Direction GREENDOWN = Direction::EAST;
    const Direction GREENRIGHT = Direction::NORTH;
    const Direction GREENLEFT = Direction::SOUTH;
    const Direction GREENUPRIGHT = Direction::NORTHWEST;
    const Direction GREENUPLEFT = Direction::SOUTHWEST;
    const Direction GREENDOWNRIGHT = Direction::NORTHEAST;
    const Direction GREENDOWNLEFT = Direction::SOUTHEAST;

    const std::vector<boardIndex> cornerIndices = {
        33, 34, 35, 49, 50, 51, 65, 66, 67,           // sw
        44, 45, 46, 60, 61, 62, 76, 77, 78,           // se
        209, 210, 211, 225, 226, 227, 241, 242, 243,  // nw
        220, 221, 222, 236, 237, 238, 252, 253, 254}; // ne

    const PieceColour initailTurn = PieceColour::RED;

    const Direction getLeft(PieceColour c)
    {
        switch (c)
        {
        case PieceColour::RED:
            return REDLEFT;
        case PieceColour::GREEN:
            return GREENLEFT;
        case PieceColour::BLUE:
            return BLUELEFT;
        case PieceColour::YELLOW:
            return YELLOWLEFT;
        default:
            return REDLEFT;
        }
    }

    const Direction getRight(PieceColour c)
    {
        switch (c)
        {
        case PieceColour::RED:
            return REDRIGHT;
        case PieceColour::GREEN:
            return GREENRIGHT;
        case PieceColour::BLUE:
            return BLUERIGHT;
        case PieceColour::YELLOW:
            return YELLOWRIGHT;
        default:
            return REDRIGHT;
        }
    }

    const Direction getUp(PieceColour c)
    {
        switch (c)
        {
        case PieceColour::RED:
            return REDUP;
        case PieceColour::GREEN:
            return GREENUP;
        case PieceColour::BLUE:
            return BLUEUP;
        case PieceColour::YELLOW:
            return YELLOWUP;
        default:
            return REDUP;
        }
    }

    const Direction getDown(PieceColour c)
    {
        switch (c)
        {
        case PieceColour::RED:
            return REDDOWN;
        case PieceColour::GREEN:
            return GREENDOWN;
        case PieceColour::BLUE:
            return BLUEDOWN;
        case PieceColour::YELLOW:
            return YELLOWDOWN;
        default:
            return REDDOWN;
        }
    }

    const Direction getUpRight(PieceColour c)
    {
        switch (c)
        {
        case PieceColour::RED:
            return REDUPRIGHT;
        case PieceColour::GREEN:
            return GREENUPRIGHT;
        case PieceColour::BLUE:
            return BLUEUPRIGHT;
        case PieceColour::YELLOW:
            return YELLOWUPRIGHT;
        default:
            return REDUPRIGHT;
        }
    }

    const Direction getUpLeft(PieceColour c)
    {
        switch (c)
        {
        case PieceColour::RED:
            return REDUPLEFT;
        case PieceColour::GREEN:
            return GREENUPLEFT;
        case PieceColour::BLUE:
            return BLUEUPLEFT;
        case PieceColour::YELLOW:
            return YELLOWUPLEFT;
        default:
            return REDUPLEFT;
        }
    }

    bool isRedStart(short i)
    {
        return i > 35 && i < 60;
    }

    bool isYellowStart(short i)
    {
        return i > 227 && i < 252;
    }

    bool isBlueStart(short i)
    {
        i = i % 16;
        return i == 1 || i == 2;
    }

    bool isGreenStart(short i)
    {
        i = i % 16;
        return i == 13 || i == 14;
    }

    bool isPawnStart(short i)
    {
        if ((i > 51 && i < 60) || (i > 227 && i < 236))
        { // red and yellow
            return true;
        }
        short rem = i % 16;
        return (i > 81 && i < 207 && (rem == 13 || rem == 2));
    }

    bool isRookStart(short i)
    {
        return i == 36 || i == 43 || i == 81 || i == 94 || i == 193 || i == 206 || i == 244 || i == 251;
    }

    bool isKnightStart(short i)
    {
        return i == 37 || i == 42 || i == 97 || i == 110 || i == 177 || i == 190 || i == 245 || i == 250;
    }

    bool isBishopStart(short i)
    {
        return i == 38 || i == 41 || i == 113 || i == 126 || i == 161 || i == 174 || i == 246 || i == 249;
    }

    bool isQueenStart(short i)
    {
        return i == 39 || i == 129 || i == 158 || i == 248;
    }

    bool isKingStart(short i)
    {
        return i == 40 || i == 145 || i == 142 || i == 247;
    }

    bool isEmptyStart(short i)
    {
        short tmp = i % 16;
        bool in14Square = (i > 32 && i < 255 && tmp != 15 && tmp != 0);
        bool notInCorners = true;
        for (boardIndex j : cornerIndices)
        {
            if (i == j)
            {
                notInCorners = false;
                break;
            }
        }
        return notInCorners && in14Square;
    }

    Square generateSquare(short i)
    {
        // using namespace boardInit;
        SquareColour sc = i % 2 == 0 ? SquareColour::DARK : SquareColour::LIGHT;
        // assign colour
        PieceColour pc = PieceColour ::NONE;
        pc = isRedStart(i) ? PieceColour::RED : isBlueStart(i) ? PieceColour::BLUE
                                            : isYellowStart(i) ? PieceColour::YELLOW
                                            : isGreenStart(i)  ? PieceColour::GREEN
                                                               : PieceColour::NONE;
        // assign type
        PieceType pt = PieceType::BLOCK;
        pt = isEmptyStart(i) ? PieceType::EMPTY : pt; // empty
        pt = isRookStart(i) ? PieceType::ROOK : isKnightStart(i) ? PieceType::KNIGHT
                                            : isBishopStart(i)   ? PieceType::BISHOP
                                                                 : pt;
        pt = isPawnStart(i) ? PieceType::PAWN : isKingStart(i) ? PieceType::KING
                                            : isQueenStart(i)  ? PieceType::QUEEN
                                                               : pt;
        return Square(sc, pc, pt);
    };

    template <typename T>
    std::vector<T> concat(std::vector<T> v1, std::vector<T> v2)
    {
        for (T i : v2)
        {
            v1.emplace_back(i);
        }
        return v1;
    }

    constexpr std::string_view PieceTypeToString(PieceType t)
    {
        using pt = PieceType;
        switch (t)
        {
        case pt::PAWN:
            return "PAWN";
        case pt::ROOK:
            return "ROOK";
        case pt::KNIGHT:
            return "KNIGHT";
        case pt::BISHOP:
            return "BISHOP";
        case pt::QUEEN:
            return "QUEEEN";
        case pt::KING:
            return "KING";
        case pt::EMPTY:
            return "EMPTY";
        default:
            return "BLOCK";
        };
    };

    constexpr std::string_view PieceColourToString(PieceColour t)
    {
        using pc = PieceColour;
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
    std::string_view pieceToStringView(Piece p)
    {
        return PieceTypeToString(p.type);
    }

    std::string moveToString(Move m)
    {
        std::string s = std::string(pieceToStringView(m.fromSquare));
        s.append(std::to_string(m.fromIndex));
        s.append(pieceToStringView(m.toSquare));
        s.append(std::to_string(m.toIndex));
        return s;
    }

    PieceColour getPrevTurn(PieceColour current) {
        switch (current) {
            case PieceColour::BLUE:
                return PieceColour::RED;
            case PieceColour::YELLOW:
                return PieceColour::BLUE;
            case PieceColour::GREEN:
                return PieceColour::YELLOW;
            case PieceColour::RED:
                return PieceColour::GREEN;
            default:
                return PieceColour::NONE;
        }
        return PieceColour::NONE;
    }

    PieceColour getNextTurn(PieceColour current) {
        switch (current) {
            case PieceColour::RED:
                return PieceColour::BLUE;
            case PieceColour::BLUE:
                return PieceColour::YELLOW;
            case PieceColour::YELLOW:
                return PieceColour::GREEN;
            case PieceColour::GREEN:
                return PieceColour::RED;
            default:
                return PieceColour::NONE;
        }
        return PieceColour::NONE;
    }
    
    void printMoveVector(std::vector<Move> mvs) {
        std::cout << "Moves: ";
        for (Move m : mvs) {
            std::cout << moveToString(m) << ", ";
        }
        std::cout << "\n";
    }
};
#endif