cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(polarssl)

file(GLOB C_FILES *.c)
file(GLOB H_FILES polarssl/*.h)

add_library(polarssl ${C_FILES} ${H_FILES})
