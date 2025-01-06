
#include<iostream>
#include<vector>
#include<algorithm>
#include<exception>
#include"board.h"
// using namespace std;
// main things:
// encoding:
// pieces 
// board (piece or square centric) -> modified bitboard
// square 
// evaluation algorithm
// search algorithm -> minimax -> alpha beta
// pseudo legal move generation
// legal move generation 
using namespace board;

class Engine : Board {
    PieceColour self;
    PieceColour oponnent;
    const int DEPTH = 4;
    private:
        int getPieceValue(Piece p) {
            switch (p.type) {
                case PieceType::PAWN:
                    return 1;
                case PieceType::ROOK:
                    return 5;
                case PieceType::BISHOP:
                    return 3;
                case PieceType::KNIGHT:
                    return 3;
                case PieceType::QUEEN:
                    return 9;
                case PieceType::KING:
                    return 200;
                default:
                    return 0;
            }
            return 0;
        }

        // returns material evaluation for all colours 
        // vector in form {r, b, y, g}
        const std::vector<float> evaluateMaterial() {
            std::vector<float> weightings {0, 0, 0, 0}; //r b y g
            auto board = getBoard();
            for (Square s : board) {
                switch (s.pieceColour) {
                    case PieceColour::RED:
                        weightings[0] += getPieceValue(s);
                        break;
                    case PieceColour::BLUE:
                        weightings[1] += getPieceValue(s);
                        break;
                    case PieceColour::YELLOW:
                        weightings[2] += getPieceValue(s);
                        break;
                    case PieceColour::GREEN:
                        weightings[3] += getPieceValue(s);
                        break;
                    default:
                        continue;
                }
            }
            return weightings;
        }

        const std::vector<float> evaluatePosition() {
            // fundamental ideas
            // pawns close to center = good
            // developed position = good
            // doubled, isolated and blocked pawns is bad
            // ideally the cost of pushing a pawn to the center but making it isolated should be greater than not pushing and reinforcing
            // TODO implement later as an extra
        }

        // vector in form {r, b, y, g}
        const std::vector<float> evaluateMobility() {
            std::vector<float> out {0, 0, 0, 0};
            auto history = getMoveHistory();
            if (history.size() < 4) {
                for (Move m : history) {
                    placeAtColourIndex(&out, m.fromSquare.pieceColour, m.totalMoves);
                }
                return out;
            }

            for (int i = 0; i < 4; i++) {
                Move m = history[history.size() - 1 - i];
                placeAtColourIndex(&out, m.fromSquare.pieceColour, m.totalMoves);
            }
            return out;
        }
     public:
        // returns a next move 
        Move chooseNextMove(std::vector<Move>) {
            // TODO ADAPT FOR MULTIPLE OPPONENT NATURE OF 4PCHESS
            // generate legal moves
            auto moves = generateLegalMoves();
            auto movesLength = moves.size();
            // store the generated move length 
            // play a move 
            float bestCutOff = 999;
            float bestEval = (float) -9999;
            Move bestMove = moves[0];
            for (Move m : moves) {
                // m.totalMoves = movesLength;
                playMove(m);
                float eval = alphaBetaMax(-9999, 9999, DEPTH);
                if (eval > bestEval) {
                    bestEval = eval;
                    bestMove = m;
                    if (eval > bestCutOff) {
                        unPlayMove();
                        break;
                    }
                }
                unPlayMove();
            }
            return bestMove;
        }

        // search to a depth
        // alpha =  lower bound
        // beta = upper bound
        // search for better move
        // if better move is found then bring alpha up to better move's score
        // if the minbound exceeds maxbound it wont be considered
        float alphaBetaMax(float alpha, float beta, int depth) {
            // TODO ADAPT FOR MULTIPLE OPPONENT NATURE OF 4PCHESS
            if (depth == 0) {
                // return quiesce
                return evaluateBoard();
            }
            auto moves = generateLegalMoves();
            for (auto move : moves) {
                playMove(move);
                float res = alphaBetaMin(alpha, beta, depth-1);
                if (res > alpha && res < beta) { // found a better move
                    alpha = res; // new lower bound is our best move
                } else if (res > alpha && res >= beta) { // our better move wont be considered by the opponent
                    unPlayMove();
                    return beta;
                }
                unPlayMove();
            }
            return alpha;
        }

        // alpha =  lower bound
        // beta = upper bound
        float alphaBetaMin(float alpha, float beta, int depth) { //tries to minimise the value
            // TODO ADAPT FOR MULTIPLE OPPONENT NATURE OF 4PCHESS
            if (depth == 0) {
                // return quiesce
                return evaluateBoard();
            } 
            auto moves = generateLegalMoves();
            for (auto move : moves) {
                playMove(move);
                float res = alphaBetaMax(alpha, beta, depth-1);
                if (res < beta) {
                    beta = res; // new min found
                    if (res < alpha) { // wont be considered
                        unPlayMove();
                        return alpha;
                    }
                }
            }
            return beta;
        }



        // evaluates current board position
        // evaluation works by calculating the advantages based on material and position and choosing one player to focus
        // basically constantly tries to bring down the best player (other than itself) and assumes others will too!
        float evaluateBoard() {
            auto material = evaluateMaterial();
            multiplyValues(&material, (float) 10); // weightings
            auto mobility = evaluateMobility();
            // auto position = evaluatePosition();
            auto advantages = layer(material, mobility); // combine into singular valus

            char selfIndex = getColourIndex(self);
            char maxAdvantageIndex = 0; // index of max advantage
            for (char i = 0; i < advantages.size(); i++) {
                if (selfIndex != i && advantages[i] > advantages[maxAdvantageIndex]) {
                    maxAdvantageIndex = i;
                }
            }
            return advantages[selfIndex] - advantages[maxAdvantageIndex];
        }        
};