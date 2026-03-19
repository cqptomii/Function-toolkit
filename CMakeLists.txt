cmake_minimum_required(VERSION 3.27)
project(Procedural)

set(CMAKE_CXX_STANDARD 17)

add_executable(Procedural main.cpp
        src/utils/Math_toolkit.hpp
        src/noise/noise.hpp
        src/noise/perlin.hpp
        src/noise/simplex.hpp
        src/landscape.hpp
        src/utils/save.hpp
        )

target_include_directories(Procedural PRIVATE ${CMAKE_SOURCE_DIR}/src)