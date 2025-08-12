cmake_minimum_required(VERSION 3.11)
project(frontend)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/parser.cpp
    src/tokenizer.cpp
)

add_executable(frontend ${SOURCES})

target_include_directories(frontend
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../shared/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../tree/include
)

target_link_libraries(frontend tree)
