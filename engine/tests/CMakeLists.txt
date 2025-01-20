cmake_minimum_required(VERSION 3.22.1)
add_compile_options(-O3 -Wall -Werror)
project(4PChess)

find_package(Catch2 3 REQUIRED)
add_executable(e benchmark.cpp)
target_link_libraries(e PRIVATE Catch2::Catch2WithMain)
set_property(TARGET e PROPERTY CXX_STANDARD 20)

