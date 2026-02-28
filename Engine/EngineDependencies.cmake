cmake_minimum_required(VERSION 4.0 FATAL_ERROR)

# include FetchContent module
include(FetchContent)

# FetchContent_Declare - options that describe how to populate content
# FetchContent_MakeAvailable - ensures dependencies have been populated

# spdlog
FetchContent_Declare(
        spdlog
        URL https://github.com/gabime/spdlog/archive/refs/tags/v1.17.0.tar.gz
)
FetchContent_MakeAvailable(
        spdlog
)
target_link_libraries(Engine PUBLIC spdlog)