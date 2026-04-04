cmake_minimum_required(VERSION 4.0 FATAL_ERROR)
project(EngineRoot)

# ─── Global C++ standard ──────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ─── Global compiler options ──────────────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4 /utf-8)
endif()

add_subdirectory(Engine)
add_subdirectory(Projects)