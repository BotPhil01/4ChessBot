
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/benchmark/catch_constructor.hpp>
#include <catch2/generators/catch_generators_range.hpp>

// #include"../helper.h"
#include"../types.h"
// #include"../playerData.h"
#include"../board.h"
#include"../engine.h"
#include"../evaluator.h"

#include<cstdint>
#include<vector>
#include<memory>

// using namespace helper;
using namespace types;
using namespace std;
// using namespace player;
using namespace board;
using namespace engine;
using namespace eval;



// void choosePlayUnPlay(Engine e, Board b) {
//     b.playMove(e.chooseNextMove());
//     b.unPlayMove();
// }

// void playUnPlay(Board b, Move m) {
//     b.playMove(m);
//     b.unPlayMove();
// }   

// TEST_CASE("pawnshift") {
//     BENCHMARK_ADVANCED("default")(Catch::Benchmark::Chronometer meter) {
//         Board b = Board();
//         meter.measure([&b] { return b.pawnShift(52); });
//     };
//     BENCHMARK_ADVANCED("reserve + tmp")(Catch::Benchmark::Chronometer meter) {
//         Board b = Board();
//         std::vector<types::boardIndex> out;
//         meter.measure([&b, &out] { return b.pawnShift4(52, out); });
//     };
//     BENCHMARK_ADVANCED("array")(Catch::Benchmark::Chronometer meter) {
//         Board b = Board();
//         std::array<types::boardIndex, 4UL> out;
//         meter.measure([&b, &out] { return b.pawnShift5(52, out); });
//     };
    
// }

TEST_CASE("balls") {
    // BENCHMARK_ADVANCED("Choose 2")(Catch::Benchmark::Chronometer meter) {
    //     Board b = Board();
    //     Engine e = Engine(b, PieceColour::RED, 2);
    //     meter.measure([&e] { return e.chooseNextMove(); });
    // };
    // BENCHMARK_ADVANCED("Choose 4")(Catch::Benchmark::Chronometer meter) {
    //     Board b = Board();
    //     Engine e = Engine(b, PieceColour::RED, 4);
    //     meter.measure([&e] { return e.chooseNextMove(); });
    // };
    // BENCHMARK_ADVANCED("Choose 8")(Catch::Benchmark::Chronometer meter) {
    //     Board b = Board();
    //     Engine e = Engine(b, PieceColour::RED, 8);
    //     meter.measure([&e] { return e.chooseNextMove(); });
    // };
    // BENCHMARK_ADVANCED("Choose 14")(Catch::Benchmark::Chronometer meter) {
    //     Board b = Board();
    //     Engine e = Engine(b, PieceColour::RED, 14);
    //     meter.measure([&e] { return e.chooseNextMove(); });
    // };
    // BENCHMARK_ADVANCED("Choose 16")(Catch::Benchmark::Chronometer meter) {
    //     Board b = Board();
    //     Engine e = Engine(b, PieceColour::RED, 16);
    //     meter.measure([&e] { return e.chooseNextMove(); });
    // };
    // BENCHMARK_ADVANCED("Choose 18")(Catch::Benchmark::Chronometer meter) {
    //     Board b = Board();
    //     Engine e = Engine(b, PieceColour::RED, 18);
    //     meter.measure([&e] { return e.chooseNextMove(); });
    // };
    // BENCHMARK_ADVANCED("Choose and play 2")(Catch::Benchmark::Chronometer meter) {
    //     Board b = Board();
    //     Engine e = Engine(b, PieceColour::RED, 2);
    //     meter.measure([&b, &e] {  return choosePlayUnPlay(e, b); } );
    // };
    // BENCHMARK_ADVANCED("Choose and play 4")(Catch::Benchmark::Chronometer meter) {
    //     Board b = Board();
    //     Engine e = Engine(b, PieceColour::RED, 2);
    //     meter.measure([&b, &e] {  return choosePlayUnPlay(e, b); } );
    // };
    // BENCHMARK_ADVANCED("Choose and play 8")(Catch::Benchmark::Chronometer meter) {
    //     Board b = Board();
    //     Engine e = Engine(b, PieceColour::RED, 2);
    //     meter.measure([&b, &e] {  return choosePlayUnPlay(e, b); } );
    // };

    BENCHMARK_ADVANCED("ALPHA BETA MAX")(Catch::Benchmark::Chronometer meter) {
        Board board = Board();
        Engine r = Engine(board, PieceColour::RED, 1);
        meter.measure([&r] {
            r.alphaBetaMax(1, -9999999.0f, 999999.0f);
        
        });
    };
    BENCHMARK_ADVANCED("ALPHA BETA MIN")(Catch::Benchmark::Chronometer meter) {
        Board board = Board();
        Engine r = Engine(board, PieceColour::RED, 1);
        meter.measure([&r] {
            r.alphaBetaMin(1, -9999999.0f, 999999.0f);
        
        });
    };

    BENCHMARK_ADVANCED("MOVE GENERATION")(Catch::Benchmark::Chronometer meter) {
        Board board = Board();
        Engine r = Engine(board, PieceColour::RED, 1);
        Engine b = Engine(board, PieceColour::BLUE, 1);
        Engine y = Engine(board, PieceColour::YELLOW, 1);
        Engine g = Engine(board, PieceColour::GREEN, 1);
        for (unsigned int i = 0; i < 4; ++i) {
            board.playMove(r.chooseNextMove());
            board.playMove(b.chooseNextMove());
            board.playMove(y.chooseNextMove());
            board.playMove(g.chooseNextMove());
        }
        vector<unique_ptr<Move>> out;
        meter.measure([&board, &out] {
            board.generateLegalMoves(PieceColour::RED, out);
        });
    };

    BENCHMARK_ADVANCED("MG PSEUDOLEGAL")(Catch::Benchmark::Chronometer meter) {
        Board board = Board();
        std::vector<std::unique_ptr<Move>> out;
        meter.measure([&board, &out] {
            board.generatePseudoLegalMoves(PieceColour::RED, out);
        });
    };

    BENCHMARK_ADVANCED("MG PSEUDOLEGAL + CLEAR")(Catch::Benchmark::Chronometer meter) {
        Board board = Board();
        std::vector<std::unique_ptr<Move>> out;
        meter.measure([&board, &out] {
            board.generatePseudoLegalMoves(PieceColour::RED, out);
            out.clear();
        });
    };


   BENCHMARK_ADVANCED("MG VIRTUAL BOARD COPY CONSTRUCTOR")(Catch::Benchmark::Chronometer meter) {
        Board board = Board();
        meter.measure([&board] {
            Board tmp = board;
        });
    };

    BENCHMARK_ADVANCED("MG COPY + VIRTUAL PLAY")(Catch::Benchmark::Chronometer meter) {
        Board board = Board();
        std::vector<std::unique_ptr<Move>> out;
        board.generatePseudoLegalMoves(PieceColour::RED, out);
        meter.measure([&out, &board] {
            Board tmp = Board(board);
            tmp.virtualPlayMove(*(out[10]));
        });
    };

   BENCHMARK_ADVANCED("MG VIRTUAL PLAY + UNPLAY")(Catch::Benchmark::Chronometer meter) {
        Board board = Board();
        std::vector<std::unique_ptr<Move>> out;
        board.generatePseudoLegalMoves(PieceColour::RED, out);
        Move m = Move(*(out[0]));
        return meter.measure([&board, &m] {
            board.virtualPlayMove(m);
            board.virtualUnPlayMove();
        });
    };

    BENCHMARK_ADVANCED("MG COPY + VIRTUAL UNPLAY")(Catch::Benchmark::Chronometer meter) {
        Board board = Board();
        std::vector<std::unique_ptr<Move>> out;
        board.generatePseudoLegalMoves(PieceColour::RED, out);
        Move m = Move(*(out[0]));
        board.virtualPlayMove(m);
        meter.measure([&board] {
            Board tmp = Board(board);
            
            tmp.virtualUnPlayMove();
        });
    };

    BENCHMARK_ADVANCED("MG KING IN CHECK")(Catch::Benchmark::Chronometer meter) {
        Board board = Board();
        meter.measure([&board] {
            board.isKingInCheck(types::PieceColour::RED);
        });
  };

    BENCHMARK_ADVANCED("BOARD EVALUATION")(Catch::Benchmark::Chronometer meter) {
        Board board = Board();
        Engine r = Engine(board, PieceColour::RED, 1);
        Engine b = Engine(board, PieceColour::BLUE, 1);
        Engine y = Engine(board, PieceColour::YELLOW, 1);
        Engine g = Engine(board, PieceColour::GREEN, 1);
        Evaluator e = Evaluator();
        for (unsigned int i = 0; i < 4; ++i) {
            board.playMove(r.chooseNextMove());
            board.playMove(b.chooseNextMove());
            board.playMove(y.chooseNextMove());
            board.playMove(g.chooseNextMove());
        }
        meter.measure([&board, &e] {
            e.getEvaluation(board, board.getPlayers());
        });
    };
}