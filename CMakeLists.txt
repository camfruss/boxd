
cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-fsanitize=address -g -Wall -Wextra")

project(boxd
    VERSION 0.0.1
    LANGUAGES CXX
)

file(GLOB_RECURSE SRC_FILES "cpp/*.cpp")
add_executable(
    boxd
    ${SRC_FILES}
)

