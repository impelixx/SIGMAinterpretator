set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

cmake_minimum_required(VERSION 3.18)
project(backend)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/lib"
)


file(
    GLOB
    SOURCES
    "src/**/*.cpp"
    "src/*.cpp"
    "src/Lexem.cpp"
    "src/Semantic.cpp"
)

set(
    PROJECT_LINK_LIBS
)

add_executable(${PROJECT_NAME} ${SOURCES})

