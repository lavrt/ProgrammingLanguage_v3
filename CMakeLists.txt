cmake_minimum_required(VERSION 3.11)
project(lang)

find_package(Boost CONFIG REQUIRED COMPONENTS program_options process filesystem)

add_subdirectory(src/core/tree)
add_subdirectory(src/core/frontend)
add_subdirectory(src/core/backend)

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
        Boost::process
        Boost::filesystem
)

# conan install . -of build -s build_type=Release --build=missing
# cmake --preset conan-release 
# cmake --build --preset conan-release -j
