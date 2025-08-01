cmake_minimum_required(VERSION 3.11)
project("languageBackend")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/headers.cpp
    src/generator.cpp
    src/asmCommands.cpp
    src/standardFunctions.cpp
)

add_executable(lang_backend ${SOURCES})
target_include_directories(lang_backend PUBLIC include)
