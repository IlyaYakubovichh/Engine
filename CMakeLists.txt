cmake_minimum_required(VERSION 4.0 FATAL_ERROR)

# C++ setup
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project
project(Engine)

add_subdirectory(Engine)
add_subdirectory(Projects)