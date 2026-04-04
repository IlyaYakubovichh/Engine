cmake_minimum_required(VERSION 4.0 FATAL_ERROR)
project(SandBox)

# ─── Sources ──────────────────────────────────────────────────────────────────
file(GLOB_RECURSE Headers CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE Sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE Shaders CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/*.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/*.comp
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Headers} ${Sources} ${Shaders})

# ─── Target ───────────────────────────────────────────────────────────────────
add_executable(SandBox ${Headers} ${Sources} ${Shaders})
target_link_libraries(SandBox PRIVATE Engine)

# SandBox-specific headers (Core, Layers)
target_include_directories(SandBox PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# ─── Post-build: copy Engine.dll next to SandBox.exe ─────────────────────────
add_custom_command(TARGET SandBox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Engine>
        $<TARGET_FILE_DIR:SandBox>
    COMMENT "Copying Engine.dll → output directory"
)