cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
project(Engine)

# ─── Sources ──────────────────────────────────────────────────────────────────
file(GLOB_RECURSE Headers CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE Sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Headers} ${Sources})

# ─── Target ───────────────────────────────────────────────────────────────────
add_library(Engine SHARED ${Headers} ${Sources})
target_precompile_headers(Engine PRIVATE Pch.h)

# ─── Compiler options ─────────────────────────────────────────────────────────
target_compile_options(Engine PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:
        /utf-8
        /wd4251   # dll-interface warnings
        /wd4275
    >
)

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
# PUBLIC — consumers of Engine (e.g. SandBox) also get these paths
target_include_directories(Engine PUBLIC
    # Engine root — for Engine.h and Pch.h
    ${CMAKE_CURRENT_SOURCE_DIR}

    # Core — Application, Window, Layer, Macros, Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core

    # Systems — Subsystem.h + File / Log / Layer / Window subsystems
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Systems

    # Renderer abstractions — Renderer, RendererAPI, Pipelines, Shaders, Image
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Renderer

    # Vulkan platform implementation
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Renderer/Platform/Vulkan
)

# ─── Dependencies ─────────────────────────────────────────────────────────────
include(Dependencies.cmake)