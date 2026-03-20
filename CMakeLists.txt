cmake_minimum_required(VERSION 4.0 FATAL_ERROR)

project(EngineRoot)

# C++23, no compiler extensions
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(Engine)
add_subdirectory(Projects)