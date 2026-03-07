cmake_minimum_required(VERSION 4.0 FATAL_ERROR)

project(Engine)

# .h and .cpp files
file(GLOB_RECURSE Headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)
file(GLOB_RECURSE Sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

# .dll target
add_library(Engine SHARED ${Sources})

# Target properties
target_precompile_headers(Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/EnginePch.h)
if(WIN32)
    set_target_properties(Engine PROPERTIES PREFIX "" OUTPUT_NAME "Engine")
endif()
target_compile_definitions(Engine PRIVATE $<$<CONFIG:Debug>:ENGINE_DEBUG>)
target_compile_definitions(Engine PRIVATE $<$<CONFIG:Debug>:ENGINE_RELEASE>)
target_compile_definitions(Engine PRIVATE ENGINE_EXPORTS)
target_include_directories(Engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Layer
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Systems
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Window
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer
)

# Engine dependencies setup
include(EngineDependencies.cmake)