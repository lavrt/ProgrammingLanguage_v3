cmake_minimum_required(VERSION 3.11)
project(tree)

set(SOURCES
    src/tree.cpp
)

add_library(tree STATIC ${SOURCES})

target_include_directories(tree
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/common
)
