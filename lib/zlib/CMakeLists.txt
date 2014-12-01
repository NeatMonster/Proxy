cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(zlib)

file(GLOB C_FILES *.c)
file(GLOB H_FILES *.h)

add_library(zlib ${C_FILES} ${H_FILES})
