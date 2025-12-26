cmake_minimum_required(VERSION 3.10)
project(newmuduo)

set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/lib)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20 -fPIC -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20 -fPIC -fsanitize=thread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fPIC -O2")

#file(GLOB SRC_LIST "./*.cpp" "./base/*.cpp")
aux_source_directory(. SRC_LIST)
aux_source_directory(base SRC_LIST)
# include_directories(${PROJECT_SOURCE_DIR})
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
file(GLOB HEADER_FILES 
     ${PROJECT_SOURCE_DIR}/*.h
)
if(HEADER_FILES)
    file(COPY ${HEADER_FILES} DESTINATION ${PROJECT_SOURCE_DIR}/include/)
endif()
target_include_directories(newmuduo
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
add_subdirectory(example)