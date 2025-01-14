cmake_minimum_required(VERSION 3.22.1)
project(4PChess)

find_package(Catch2 3 REQUIRED)

add_executable(e tests.cpp)
target_link_libraries(e PRIVATE Catch2::Catch2WithMain)
