cmake_minimum_required(VERSION 3.27)
project(Procedural)

set(CMAKE_CXX_STANDARD 17)

add_executable(Procedural main.cpp
        src/utils/Math_toolkit.hpp
        src/utils/Math_toolkit.cpp)
