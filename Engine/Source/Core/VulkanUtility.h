//
// Created by ilya on 01.03.2026.
//

#ifndef ENGINE_VULKANUTILS_H
#define ENGINE_VULKANUTILS_H

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#define VULKAN_CHECK(res)                                                                       \
    do {                                                                                        \
        VkResult err = res;                                                                     \
        if (err) {                                                                              \
            ENGINE_LOG_ERROR("Vulkan validation layer", "Error: {}", string_VkResult(err));     \
        }                                                                                       \
    } while (0)

#endif //ENGINE_VULKANUTILS_H