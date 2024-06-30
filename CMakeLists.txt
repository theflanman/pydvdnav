cmake_minimum_required(VERSION 3.29)
project(pydvdnav)

set(CMAKE_CXX_STANDARD 20)

find_package(Python3 COMPONENTS Interpreter Development)

# Libraries
add_subdirectory(src)

set_target_properties(
        pydvdnav
        PROPERTIES
        PREFIX ""
        OUTPUT_NAME "pydvdnav"
)
