#ifndef PARSER_H
#define PARSER_H

#include "board.h"
#include "move.h"
#include <string_view>

namespace parser {
    using std::operator""sv;

    // js coords are 14x14 indexed top left
    // our coords are 16x16 grid of 4x4 bit words
    // js program sends data in form of chars
    // 3 types of inputs from js
    // Mn|n|n|nM
    // Sn|nS

    class Parser {
        board::Board &m_board;
        public:
        Parser(board::Board &board) :
            m_board(board)
        {}

        std::pair<int, int> fromJCoords(std::pair<int, int> jsCoords) {
            jsCoords.first++;
            jsCoords.second++;
            return jsCoords;
        }

        // converts 16x16 coords to 14x14 javascript coords
        std::pair<int, int> toJCoords(std::pair <int, int> coords) {
            coords.first -= 1;
            coords.second -= 1;
            return coords;
        }

        // gives a js output for a cpp move
        void parseOutput(move::Move m) {
            std::pair<int, int> fromCoords = toJCoords(m.unarySrcBoard.to16RC());
            std::pair<int, int> toCoords = toJCoords(m.destBoard.to16RC());

            std::cout << "M" << fromCoords.first << "|" << fromCoords.second << "|" << toCoords.first << "|" << toCoords.second << "M" << std::endl;
        }

        // gives js output for a finished engine
        void parseOutput(types::PieceColour checkmatedColour) {
            std::cout << "C" << helper::colourToChar(checkmatedColour) << "C" << std::endl;
        }

        // move::Move parseJsMove(std::string jMove) {
        //     jMove = jMove.substr(jMove.find_first_of("M") + 1, jMove.find_last_of("M"));
        //     // extract from std::string
        //     assert(jMove.size() > 3);
        //     std::vector<int> coordinates;
        //     size_t i = jMove.find('|', 0);
        //     while (i != jMove.npos) {
        //         coordinates.emplace_back(stoi(jMove.substr(0, i))); // place at end
        //         jMove = jMove.substr(i+1, jMove.size() - i);
        //         i = jMove.find('|', 0);
        //     }

        //     if (jMove.compare("") != 0) {
        //         coordinates.emplace_back(stoi(jMove));                
        //     }
        //     assert(coordinates.size() == 4);
        //     // reflect across x axis
        //     coordinates[1] = 13 - coordinates[1];
        //     coordinates[3] = 13 - coordinates[3];

        //     // add padding to convert from 14x14 to 16x18
        //     coordinates[0] += 1;
        //     coordinates[2] += 1;
        //     coordinates[1] += 2;
        //     coordinates[3] += 2;


        //     std::pair<int, int> from = std::pair(coordinates[0], coordinates[1]);
        //     std::pair<int, int> to = std::pair(coordinates[2], coordinates[3]);

        //     return m_board.indicesToMove(from, to);
        // }


        // these are fine
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
                    return -1;
            }
            return -1;
        }

        // cli move comes in [a-n][1-14][a-n][1-14]
        // algebraic move
        move::Move parseCliMove(std::string_view input) {
            if (input.length() < 4) {
                return move::Move();
            }
            // break down the input into indices
            const uint8_t fromX = asc2PadCol(input[0]);
            input = input.substr(1, input.length());
            const unsigned char alphaIndex = input.find_first_of("abcdefghijklmopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"sv);
            if (alphaIndex == input.npos) {
                return move::Move();
            }
            const uint8_t fromY = asc2PadRow(input.substr(0, alphaIndex));
            const uint8_t toX = asc2PadCol(input[alphaIndex]);
            const uint8_t toY = asc2PadRow(input.substr(alphaIndex + 1, input.length()));
            if (
                    fromX < 0 || fromY < 0 ||
                    toX < 0 || toY < 0) {
                return move::Move();
            }
            // return board.indicesToMove(toIndex(fromX, fromY), toIndex(toX, toY));
            return m_board.indicesToMove(std::pair(fromX, fromY), std::pair(toX, toY));
        }
    };
}

#endif
