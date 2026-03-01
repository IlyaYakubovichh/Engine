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

# glfw (API for platform independent window creation)
FetchContent_Declare(
        glfw
        URL https://github.com/glfw/glfw/archive/refs/tags/3.4.tar.gz
)
FetchContent_MakeAvailable(
        glfw
)
target_link_libraries(Engine PUBLIC glfw)

# Vulkan
find_package(Vulkan COMPONENTS shaderc_combined)
if(Vulkan_FOUND)
    target_link_libraries(Engine PUBLIC
            Vulkan::Vulkan
            Vulkan::shaderc_combined
    )
endif()

# vkbootstrap (utility that simplifies vulkan initialization)
FetchContent_Declare(
        vkbootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
        GIT_TAG main
)
FetchContent_MakeAvailable(
        vkbootstrap
)
target_link_libraries(Engine PUBLIC vk-bootstrap::vk-bootstrap)
