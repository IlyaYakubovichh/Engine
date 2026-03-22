#pragma once

// ─── Core ─────────────────────────────────────────────────────────────────────
#include "Source/Core/Utils.h"
#include "Source/Core/Macros.h"
#include "Source/Core/Layer.h"
#include "Source/Core/Window.h"
#include "Source/Core/Application.h"

// ─── Systems ──────────────────────────────────────────────────────────────────
#include "Source/Core/Systems/LogSystem/LogSystem.h"
#include "Source/Core/Systems/FileSystem/FileSystem.h"
#include "Source/Core/Systems/LayerSystem/LayerSystem.h"
#include "Source/Core/Systems/WindowSystem/WindowSystem.h"
#include "Source/Core/Systems/VulkanSystem/VulkanSystem.h"

// ─── Renderer ─────────────────────────────────────────────────────────────────
#include "Source/Renderer/Image.h"
#include "Source/Renderer/Vulkan/VulkanImage.h"
#include "Source/Renderer/Shader.h"
#include "Source/Renderer/ShaderLibrary.h"
#include "Source/Renderer/Vulkan/VulkanShader.h"
#include "Source/Renderer/RendererAPI.h"
#include "Source/Renderer/Renderer.h"
#include "Source/Renderer/Vulkan/VulkanRendererAPI.h"