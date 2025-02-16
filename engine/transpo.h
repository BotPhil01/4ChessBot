#include"types.h"
#include"hasher.h"
#include"playerData.h"
#include<cmath>
#ifndef TRANSPO_H
#define TRANSPO_H

namespace transpo {
    enum class Node {
        NONE,
        PV,
        ALL,
        CUT,
    };
    char nodeToChar(Node n) {
        switch (n) {
            case Node::NONE:
                return 'N';
                break;
            case Node::PV:
                return 'P';
                break;
            case Node::ALL:
                return 'A';
                break;
            case Node::CUT:
                return 'C';
                break;
            default:
                assert(false);
        }
        assert(false);
    }
    struct TableData {
        bool occupied;
        Node type;
        int eval;
        int age;
        unsigned int depth;
        std::uint64_t key;
        types::Move bestMove;
    };
    hasher::Hasher HASHER = hasher::Hasher();
    constexpr TableData EMPTY = {false, Node::PV, 0, 0, 0, 0, types::Move()};
    typedef std::array<std::reference_wrapper<player::Player>, 4UL> PlayerArrayType;
    class TranspositionTable {
        unsigned int ctr = 0;
        static const unsigned int SIZE = 1024;
        std::array<TableData, SIZE> arr;
        unsigned int ageDiff = 3;
        
        unsigned int findReplacement(std::uint64_t hash, int age, unsigned int depth) {
            unsigned int index = hash % SIZE;
            if (arr[index].depth < depth || std::abs(age - arr[index].age) > ageDiff) {
                return index;
            }
            return SIZE;
        }
        public:
        TranspositionTable()
        {
            arr.fill(EMPTY);
        }

        void store(types::Move bestMove, int eval,  int age, unsigned int depth, Node type, PlayerArrayType players) {
            assert(bestMove.fromIndex() != 300 && bestMove.toIndex() != 300);
            age++;
            std::uint64_t hash = HASHER.hashPosition(players);
            // find the hash
            int index = findReplacement(hash, age, depth);
            if (index != SIZE) {
                arr[index] = {true, type, eval, age, depth, hash, bestMove};
            }
        }

        bool contains(PlayerArrayType players) const {
            std::uint64_t hash = HASHER.hashPosition(players);
            unsigned int index = hash % SIZE;
            return arr[index].key == hash;
        }

        TableData find(PlayerArrayType players) {
            std::uint64_t hash = HASHER.hashPosition(players);
            unsigned int index = hash % SIZE;
            if (arr[index].key == hash) {
                return arr[index];
            }
            return EMPTY;
        }
    };
}

#endif