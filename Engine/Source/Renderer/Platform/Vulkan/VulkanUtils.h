#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include "Log/LogSystem.h"

/**
 * @brief Evaluates a VkResult and logs an error if it is not VK_SUCCESS.
 */
#define VK_CHECK(res)                                                               \
    do {                                                                            \
        const VkResult _vkr = (res);                                                \
        if (_vkr != VK_SUCCESS) {                                                   \
            ENGINE_LOG_ERROR("Vulkan", "VkResult: {}", string_VkResult(_vkr));      \
        }                                                                           \
    } while (0)