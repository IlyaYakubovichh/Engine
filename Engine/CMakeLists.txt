cmake_minimum_required(VERSION 4.0 FATAL_ERROR)

project(Engine)

# .h and .cpp files
file(GLOB_RECURSE Headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)
file(GLOB_RECURSE Sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

# .dll target
add_library(Engine SHARED ${Sources})

# Target properties
if(WIN32)
    set_target_properties(Engine PROPERTIES PREFIX "" OUTPUT_NAME "Engine")     # remove lib Prefix
endif()
target_compile_definitions(Engine PRIVATE ENGINE_EXPORTS)                       # macro definition
target_include_directories(Engine PUBLIC                                        # include directories
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core
)

# Engine dependencies setup
include(EngineDependencies.cmake)