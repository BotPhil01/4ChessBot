
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/benchmark/catch_constructor.hpp>
#include <catch2/generators/catch_generators_range.hpp>

// #include"../helper.h"
#include"../types.h"
// #include"../playerData.h"
#include"../board.h"
#include"../engine.h"

#include<cstdint>

// using namespace helper;
using namespace types;
// using namespace std;
// using namespace player;
using namespace board;
using namespace engine;

void choosePlayUnPlay(Engine e, Board b) {
    b.playMove(e.chooseNextMove());
    b.unPlayMove();
}

void playUnPlay(Board b, Move m) {
    b.playMove(m);
    b.unPlayMove();
}   

TEST_CASE("balls") {
    
    BENCHMARK_ADVANCED("Play move")(Catch::Benchmark::Chronometer meter) {
        Move exp = Move(52, 68, 0, PieceType::PAWN, PieceColour::RED, PieceType::EMPTY, PieceColour::NONE);
        Board exp1 = Board();
        meter.measure([&] {  return playUnPlay(exp1, exp); } );
    };

    BENCHMARK_ADVANCED("Choose 2")(Catch::Benchmark::Chronometer meter) {
        Board b = Board();
        Engine e = Engine(b, PieceColour::RED, 2);
        meter.measure([&e] { return e.chooseNextMove(); });
    };
    BENCHMARK_ADVANCED("Choose 4")(Catch::Benchmark::Chronometer meter) {
        Board b = Board();
        Engine e = Engine(b, PieceColour::RED, 2);
        meter.measure([&e] { return e.chooseNextMove(); });
    };
    BENCHMARK_ADVANCED("Choose 8")(Catch::Benchmark::Chronometer meter) {
        Board b = Board();
        Engine e = Engine(b, PieceColour::RED, 2);
        meter.measure([&e] { return e.chooseNextMove(); });
    };

    BENCHMARK_ADVANCED("Choose and play 2")(Catch::Benchmark::Chronometer meter) {
        Board b = Board();
        Engine e = Engine(b, PieceColour::RED, 2);
        meter.measure([&b, &e] {  return choosePlayUnPlay(e, b); } );
    };
    BENCHMARK_ADVANCED("Choose and play 4")(Catch::Benchmark::Chronometer meter) {
        Board b = Board();
        Engine e = Engine(b, PieceColour::RED, 2);
        meter.measure([&b, &e] {  return choosePlayUnPlay(e, b); } );
    };
    BENCHMARK_ADVANCED("Choose and play 8")(Catch::Benchmark::Chronometer meter) {
        Board b = Board();
        Engine e = Engine(b, PieceColour::RED, 2);
        meter.measure([&b, &e] {  return choosePlayUnPlay(e, b); } );
    };
}