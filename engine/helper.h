#include"types.h"
#include<cassert>
#include<vector>
#include<utility>
using namespace types;
using namespace std;
#ifndef HELPER_H
#define HELPER_H

// helper functions and constants
namespace helper
{
    
    const PieceColour RED = PieceColour::RED;
    const PieceColour BLUE = PieceColour::BLUE;
    const PieceColour YELLOW = PieceColour::YELLOW;
    const PieceColour GREEN = PieceColour::GREEN;
    const Direction NORTH = Direction::NORTH;
    const Direction EAST = Direction::EAST;
    const Direction SOUTH = Direction::SOUTH;
    const Direction WEST = Direction::WEST;
    
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
    
    const char PADDEDCOLS = 16;
    const char PADDEDROWS = 18;
    
    const std::vector<PieceColour> playableColours {RED, BLUE, YELLOW, GREEN};
    const vector<PieceType> playablePieces {PieceType::PAWN, PieceType::ROOK, PieceType::KNIGHT, PieceType::BISHOP, PieceType::QUEEN, PieceType::KING};

    const std::vector<boardIndex> cornerIndices = {
        33, 34, 35, 49, 50, 51, 65, 66, 67,           // sw
        44, 45, 46, 60, 61, 62, 76, 77, 78,           // se
        209, 210, 211, 225, 226, 227, 241, 242, 243,  // nw
        220, 221, 222, 236, 237, 238, 252, 253, 254}; // ne

    const PieceColour initailTurn = PieceColour::RED;



    const int indexFromType(PieceType t) {
        for (int i = 0; i < playablePieces.size(); ++i) {
            if (playablePieces[i] == t) {
                return i;
            }
        }
        throw invalid_argument("Wrong type provided to indexFromType");
    }

    constexpr int indexFromColour(PieceColour const c) {
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

    constexpr Direction getLeft(PieceColour const c)
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

    constexpr Direction getRight(PieceColour const c)
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

    constexpr Direction getUp(PieceColour const c)
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

    constexpr Direction getDown(PieceColour const c)
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

    constexpr Direction getUpRight(PieceColour const c)
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

    constexpr Direction getUpLeft(PieceColour const c)
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
    
    const bool indexInCorners(boardIndex i) {
        for (auto index : cornerIndices) {
            if (index == i) {
                return true;
            }
        }
        return false;
    }
    
    constexpr bool indexOnBoard(boardIndex const i) {
        int rem = i % 16;
        return (i > 35 && i < 252 && rem != 0 && rem != 15 && !indexInCorners(i));
    }

    constexpr bool isRedStart(boardIndex const i)
    {
        return (i > 35 && i < 44) || (i > 51 && i < 60);
    }

    constexpr bool isYellowStart(boardIndex const i)
    {
        return (i > 227 && i < 236) || (i > 243 && i < 252);
    }

    constexpr bool isBlueStart(boardIndex const i)
    {
        short rem = i % 16;
        return (rem == 1 || rem == 2) && i < 195 && i > 80;
    }

    constexpr bool isGreenStart(boardIndex const i)
    {
        short rem = i % 16;
        return (rem == 13 || rem == 14) && i > 92 && i < 207;
    }

    constexpr bool isPawnStart(boardIndex const i)
    {
        if ((i > 51 && i < 60) || (i > 227 && i < 236))
        { // red and yellow
            return true;
        }
        short rem = i % 16;
        return (i > 81 && i < 207 && (rem == 13 || rem == 2));
    }

    constexpr bool isRookStart(boardIndex const i)
    {
        return i == 36 || i == 43 || i == 81 || i == 94 || i == 193 || i == 206 || i == 244 || i == 251;
    }

    constexpr bool isKnightStart(boardIndex const i)
    {
        return i == 37 || i == 42 || i == 97 || i == 110 || i == 177 || i == 190 || i == 245 || i == 250;
    }

    constexpr bool isBishopStart(boardIndex const i)
    {
        return i == 38 || i == 41 || i == 113 || i == 126 || i == 161 || i == 174 || i == 246 || i == 249;
    }

    constexpr bool isQueenStart(boardIndex const i)
    {
        return i == 39 || i == 129 || i == 158 || i == 248;
    }

    constexpr bool isKingStart(boardIndex const i)
    {
        return i == 40 || i == 145 || i == 142 || i == 247;
    }

    const bool isEmptyStart(boardIndex const i)
    {
        short tmp = i % 16;
        bool in14Square = (i > 32 && i < 255 && tmp != 15 && tmp != 0);
        bool notInCorners = true;
        
        for (int j = 0; j < cornerIndices.size() ; ++j) {
            boardIndex tmpIndex = cornerIndices[j];
            if (i == tmpIndex)
            {
                notInCorners = false;
                break;
            }
        }
        return notInCorners && in14Square;
    }

    Square generateSquare(short i)
    {
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
        return Square(pt != PieceType::BLOCK, pt, pc);
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

    template <typename T>
    // layers vectors ontop of each other with + 
    // vectors must have same size
    std::vector<T> layer(std::vector<T> v1, std::vector<T> v2) {
        const size_t s1 = v1.size();
        const size_t s2 = v2.size();
        assert(s1 == s2);
        for (int i = 0; i < v1.size(); i++) {
            v1[i] += v2[i];
        }
        return v1;
    }

    template<typename T>
    void multiplyValues(std::vector<T> *v, T f) {
        for (int i = 0; i < (*v).size(); ++i) {
            (*v)[i] *= f;
        }
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
        std::string s = "";
        s.append(std::to_string(m.fromIndex()));
        s.append(std::to_string(m.toIndex()));
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
    
    // void printMoveVector(std::vector<Move> mvs) {
    //     std::cout << "Moves: ";
    //     for (Move m : mvs) {
    //         std::cout << moveToString(m) << ", ";
    //     }
    //     std::cout << "\n";
    // }

    // TODO DELETE AND REFACTOR
    char getColourIndex(PieceColour c) {
        switch (c) {
            case PieceColour::RED:
                return 0;
            case PieceColour::BLUE:
                return 1;
            case PieceColour::YELLOW:
                return 2;
            case PieceColour::GREEN:
                return 3;
            default:
                throw std::invalid_argument("Invalid colour argument in getColourIndex colour: " + std::string(PieceColourToString(c)) + "\n");
        }
        return -1;
    }

    PieceColour getColourFromIndex(char i) {
        switch (i) {
            case 0:
                return PieceColour::RED;
            case 1:
                return PieceColour::BLUE;
            case 2:
                return PieceColour::YELLOW;
            case 3:
                return PieceColour::GREEN;
            default:
                std::string s{i};
                throw std::invalid_argument("Invalid indeex argument in getColourFromIndex index: " + s + "\n");
        }
        return PieceColour::NONE;
    }
    // assumes there is a valid non diagonal straight path towards tgt from src
    Direction getDirection(boardIndex src, boardIndex tgt) {
        short diff = tgt - src;
        if (diff > 0) {
            if (diff > PADDEDCOLS) {
                return Direction::NORTH;
            }
            return Direction::EAST;
        }
        if (-diff > PADDEDCOLS) {
            return Direction::SOUTH;
        }
        return Direction::WEST;
    }

    // TODO DELETE AND REFACTOR
    // takes vector of length 4 a colour to has with and a value to place in vector
    void placeAtColourIndex(std::vector<float> * const ve, PieceColour c, float va) {
        (*ve)[getColourIndex(c)] = va;
    }

    boardIndex shiftOne(boardIndex i, Direction d) {
        switch (d) {
            case Direction::NORTH: 
                return i + 16;
            case Direction::NORTHEAST: 
                return i + 17;
            case Direction::EAST:
                return i + 1;
            case Direction::SOUTHEAST:
                return i - 15;
            case Direction::SOUTH:
                return i - 16;
            case Direction::SOUTHWEST:
                return i - 17;
            case Direction::WEST:
                return i - 1;
            case Direction::NORTHWEST:
                return i + 15;
            default:
                return i;
        };
    }

    // returns boardIndex from 16 * 18  coordinates
    boardIndex toIndex( unsigned char c, unsigned char r) { 
        return (r * 16 + c);
    }

    // translates a boardIndex to a padded row column pair
    pair<int, int> to16RC(boardIndex i) {
        int x = (i % 16);
        int y = (i - x) / 16;
        return std::pair(x,y);
    }

};

#endif