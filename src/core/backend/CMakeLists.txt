cmake_minimum_required(VERSION 3.11)
project(backend)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/headers.cpp
    src/generator.cpp
    src/asmCommands.cpp
    src/standardFunctions.cpp
)

add_executable(backend ${SOURCES})

target_include_directories(backend
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../tree/include
)

target_link_libraries(backend tree)
