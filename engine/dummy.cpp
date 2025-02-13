#include"types.h"
#include<array>
#include<iostream>
enum class Node {
    PV,
    ALL,
    CUT,
};
struct TableData {
    bool occupied;
    Node type;
    int eval;
    unsigned int age;
    unsigned int depth;
    std::uint64_t key;
    types::Move bestMove;
};
int main() {
    std::cout << sizeof(TableData) << std::endl;
    return 0;    
}