cmake_minimum_required(VERSION 3.3)
project(fwtool_project)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(deps)
add_subdirectory(src)
