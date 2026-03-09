cmake_minimum_required(VERSION 3.11)
project(lang)

find_package(Boost CONFIG REQUIRED COMPONENTS program_options)

add_subdirectory(tree)
add_subdirectory(frontend)
add_subdirectory(backend)

add_executable(compile
    src/main.cpp
    src/app/cli/cli.cpp
    src/app/proc/proc.cpp
)

target_include_directories(compile
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app/cli
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app/proc
)

target_link_libraries(compile
    PRIVATE
        Boost::program_options
)
