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
        static const unsigned int SIZE = 256;
        std::array<TableData, SIZE> arr;
        unsigned int findIndex(std::uint64_t hash) const {
            unsigned int index = hash % SIZE;
            unsigned int counter = 0;
            while (arr[index].occupied && counter++ != SIZE) {
                if (arr[index++].key == hash) {
                    return index;
                }
                index = index % SIZE;
            }
            if (counter == SIZE) {
                return SIZE;
            }
            return index;
        }
        unsigned int findFirstOld(std::uint64_t hash) const {
            unsigned int index = hash % SIZE;
            unsigned int counter = 0;
            while (arr[index].occupied && age - arr[index].age < AGEGAP) {
                index = ++index % SIZE;
            }
            return index;
        }
        public:
        TranspositionTable() {
            arr.fill(EMPTY);
        }
        
        void store(types::Move bestMove, int eval, unsigned int depth, Node type, PlayerArrayType players) {
            ctr++;
            // std::cout << "Storing node: " << nodeToChar(type) << "\n";
            age++;
            std::uint64_t hash = HASHER.hashPosition(players);
            // find the hash
            unsigned int index = findIndex(hash);
            if (index != SIZE) {
                arr[index] = {true, type, eval, age, depth, hash, bestMove};
            } else {
                // if unsuccessful find the first old one
                index = findFirstOld(hash);
                arr[index] = {true, type, eval, age, depth, hash, bestMove};
            }
            // std::cout << "Transposition table size: " << ctr << "\n";
        }

        bool contains(PlayerArrayType players) {
            std::uint64_t hash = HASHER.hashPosition(players);
            unsigned int index = hash % SIZE;
            unsigned int counter = 0;
            while (arr[index].occupied && counter++ != SIZE) {
                if (arr[index++].key == hash) {
                    return true;
                }
                index = index % SIZE;
            }
            return false;
        }

        TableData find(PlayerArrayType players) {
            std::uint64_t hash = HASHER.hashPosition(players);
            unsigned int index = hash % SIZE;
            unsigned int counter = 0;
            while (arr[index].occupied && counter++ != SIZE) {
                if (arr[index].key == hash) {
                    TableData out = arr[index];
                    (arr[index]).age = age;
                    return out;
                }
                index = (index + 1) % SIZE;
            }
            return EMPTY;
        }
    };
}

#endif