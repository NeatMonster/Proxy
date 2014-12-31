cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(Proxy)

add_subdirectory(lib/json11)
add_subdirectory(lib/polarssl)
add_subdirectory(lib/zlib)

include_directories(lib/)
include_directories(lib/polarssl)

include_directories(src/network)
include_directories(src/packet)
include_directories(src/proxy)
include_directories(src/util)

file(GLOB_RECURSE NETWORK_C src/network/*.cpp)
file(GLOB_RECURSE NETWORK_H src/network/*.h)
file(GLOB_RECURSE PACKET_C src/packet/*.cpp)
file(GLOB_RECURSE PACKET_H src/packet/*.h)
file(GLOB_RECURSE PROXY_C src/proxy/*.cpp)
file(GLOB_RECURSE PROXY_H src/proxy/*.h)
file(GLOB_RECURSE UTIL_C src/util/*.cpp)
file(GLOB_RECURSE UTIL_H src/util/*.h)

set(SOURCE_FILES ${NETWORK_C} ${PACKET_C} ${PROXY_C} ${UTIL_C} ${WORLD_C})
set(HEADER_FILES ${NETWORK_H} ${PACKET_H} ${PROXY_H} ${UTIL_H} ${WORLD_H})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -pedantic -Wextra")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pthread")
endif()
add_executable(Proxy ${SOURCE_FILES} ${HEADER_FILES})

source_group("Network" FILES ${NETWORK_C} ${NETWORK_H})
source_group("Packet" FILES ${PACKET_C} ${PACKET_H})
source_group("Proxy" FILES ${PROXY_C} ${PROXY_H})
source_group("Util" FILES ${UTIL_C} ${UTIL_H})
source_group("World" FILES ${WORLD_C} ${WORLD_H})

target_link_libraries(Proxy json11)
target_link_libraries(Proxy polarssl)
target_link_libraries(Proxy zlib)
