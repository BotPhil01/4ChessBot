#include"types.h"
#include"hasher.h"
#include"playerData.h"
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
            Default:
                assert(false);
        }
        assert(false);
    }
    struct TableData {
        bool occupied;
        Node type;
        int eval;
        unsigned int age;
        unsigned int depth;
        std::uint64_t key;
        types::Move bestMove;
    };
    hasher::Hasher HASHER = hasher::Hasher();
    constexpr TableData EMPTY = {false, Node::PV, 0, 0, 0, 0, types::Move()};
    typedef std::array<const std::reference_wrapper<player::Player>, 4UL> PlayerArrayType;
    class TranspositionTable {
        unsigned int ctr = 0;
        unsigned int age = 0;
        const unsigned int AGEGAP = 50;
        static const unsigned int SIZE = 1024;
        std::array<TableData, SIZE> arr;
        

        unsigned int findReplacement(std::uint64_t hash, unsigned int depth) {
            // always replace
            unsigned int index = hash % SIZE;
            if (arr[index].depth < depth) {
                return index;
            }
            return SIZE;
        }
        public:
        TranspositionTable() {
            arr.fill(EMPTY);
        }

        void store(types::Move bestMove, int eval, unsigned int depth, Node type, PlayerArrayType players) {
            assert(bestMove.fromIndex() != 300 && bestMove.toIndex() != 300);
            age++;
            std::uint64_t hash = HASHER.hashPosition(players);
            // find the hash
            int index = findReplacement(hash, depth);
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