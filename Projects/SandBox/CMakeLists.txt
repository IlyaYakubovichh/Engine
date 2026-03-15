cmake_minimum_required(VERSION 4.0 FATAL_ERROR)

project(SandBox)

# .h and .cpp files
file(GLOB_RECURSE Headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)
file(GLOB_RECURSE Sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)  
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Headers} ${Sources})

# .exe target
add_executable(SandBox ${Headers} ${Sources})

# Target properties
target_compile_definitions(SandBox PRIVATE ENGINE_IMPORTS)                        # macro definition
target_link_libraries(SandBox PRIVATE Engine)                                     # linking Engine
target_include_directories(SandBox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)    # include directories

# Commands
add_custom_command(TARGET SandBox PRE_BUILD                                       # copy .dll into .exe dir
        COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:SandBox> $<TARGET_FILE:Engine>
        COMMAND_EXPAND_LISTS
)