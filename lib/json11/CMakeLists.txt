cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(json11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
add_library(json11 json11.cpp json11.hpp)
