cmake_minimum_required(VERSION 4.0 FATAL_ERROR)
project(Engine)

# ─── Sources ──────────────────────────────────────────────────────────────────
file(GLOB_RECURSE Headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)
file(GLOB_RECURSE Sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Headers} ${Sources})

# ─── Target ───────────────────────────────────────────────────────────────────
add_library(Engine SHARED ${Headers} ${Sources})
target_precompile_headers(Engine PRIVATE Pch.h)

# Strip default "lib" prefix on Windows
if(WIN32)
    set_target_properties(Engine PROPERTIES PREFIX "" OUTPUT_NAME "Engine")
endif()

# ─── Definitions ──────────────────────────────────────────────────────────────
target_compile_definitions(Engine PRIVATE
    ENGINE_EXPORTS
    $<$<CONFIG:Debug>:ENGINE_DEBUG>
    $<$<CONFIG:Release>:ENGINE_RELEASE>
)

# ─── Include directories ──────────────────────────────────────────────────────
target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Systems
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer
)

# ─── Dependencies ─────────────────────────────────────────────────────────────
include(Dependencies.cmake)