#include"types.h"
#include"board.h"
#include<array>
#include<cstdint>
#include<algorithm>
#ifndef HASHER_H
#define HASHER_H

namespace hasher {
    class Hasher {
        private:
        std::array<std::uint64_t, 3456> hashValues;
        const std::uint64_t seed = 0x0125afbded493934;
        typedef struct ranAgg {
            std::uint64_t a;
            std::uint64_t b;
            std::uint64_t c;
            std::uint64_t d;
        } ranAgg;
        
        ranAgg ranInit(std::uint64_t seed) {
            std::uint64_t i;
            ranAgg out;
            out.a = 0xf1ea5eed;
            out.b = out.c = out.d = seed;
            for (unsigned int i = 0; i < 20; ++i) {
                out = ranShift(out);
            }
            return out;
        }

        #define rotate(x, k) ((x << k) | (x >> (64 - k)))
        ranAgg ranShift(ranAgg in) {
            std::uint64_t e = in.a - rotate(in.b, 7);
            in.a = in.b ^ rotate(in.c, 13);
            in.b = in.c ^ rotate(in.d, 37);
            in.c = in.d + e;
            in.d = e + in.a;
            return in;
        }

        public:
        Hasher() {
            static ranAgg agg = ranInit(seed);
            std::generate(hashValues.begin(), hashValues.end(), [this](){
                agg = ranShift(agg);
                return agg.d;
            });
        }
        
        
        std::uint64_t hashPosition(std::array<const std::reference_wrapper<player::Player>, 4UL> players) {
            std::uint64_t hash = 0x0;
            std::for_each(players.begin(), players.end(), [&hash, this](std::reference_wrapper<player::Player> p){
                player::Player player = p.get();
                unsigned int colourIndex = helper::indexFromColour(player.colour());
                std::array<const std::reference_wrapper<std::set<types::boardIndex>>, 6UL> pieces = player.getPieces();
                for (unsigned int pieceIndex = 0; pieceIndex < pieces.size(); ++pieceIndex) {
                    std::set<types::boardIndex> set = pieces[pieceIndex].get();
                    unsigned int offset = colourIndex * pieceIndex * 144;
                    std::for_each(set.begin(), set.end(), [&offset, &hash, this](types::boardIndex index){
                        hash ^= hashValues[index + offset];
                    });
                }
            });
            return hash;
        }
    };
}

#endif