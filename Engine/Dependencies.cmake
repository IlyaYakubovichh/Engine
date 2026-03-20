include(FetchContent)

# ─── Declarations ─────────────────────────────────────────────────────────────
FetchContent_Declare(spdlog
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.17.0.tar.gz
)
FetchContent_Declare(glfw
    URL https://github.com/glfw/glfw/archive/refs/tags/3.4.tar.gz
)
FetchContent_Declare(glm
    URL https://github.com/g-truc/glm/archive/refs/tags/1.0.3.tar.gz
)
FetchContent_Declare(vma
    URL https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/archive/refs/tags/v3.3.0.tar.gz
)
FetchContent_Declare(vkbootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
    GIT_TAG        v1.3.296
)

FetchContent_MakeAvailable(spdlog glfw glm vma vkbootstrap)

# ─── Vulkan (system package) ──────────────────────────────────────────────────
find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

# ─── Link ─────────────────────────────────────────────────────────────────────
target_link_libraries(Engine PUBLIC
    spdlog
    glfw
    glm
    GPUOpen::VulkanMemoryAllocator
    vk-bootstrap::vk-bootstrap
    Vulkan::Vulkan
    Vulkan::shaderc_combined
)

# ─── IDE grouping ─────────────────────────────────────────────────────────────
foreach(dep IN ITEMS spdlog glfw vk-bootstrap glm VulkanMemoryAllocator)
    if(TARGET ${dep})
        set_target_properties(${dep} PROPERTIES FOLDER "EngineDependencies")
    endif()
endforeach()