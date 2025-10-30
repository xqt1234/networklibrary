cmake_minimum_required(VERSION 3.10)
project(newmuduo)

set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20 -fPIC")

#file(GLOB SRC_LIST "./*.cpp" "./base/*.cpp")
aux_source_directory(. SRC_LIST)
aux_source_directory(base SRC_LIST)
include_directories(${PROJECT_SOURCE_DIR}/base)
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})