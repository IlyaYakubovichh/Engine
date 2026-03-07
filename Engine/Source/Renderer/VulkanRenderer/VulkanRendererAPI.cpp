#include "VulkanRendererAPI.h"

namespace Engine {

    VulkanRendererAPI::VulkanRendererAPI() {
        mAPI = API::Vulkan;
    }

    void VulkanRendererAPI::Start()           {}
    void VulkanRendererAPI::Shutdown()        {}
    void VulkanRendererAPI::BeginFrame()      {}
    void VulkanRendererAPI::EndFrame()        {}
    void VulkanRendererAPI::BeginRenderPass() {}
    void VulkanRendererAPI::EndRenderPass()   {}
    void VulkanRendererAPI::Present()         {}

} // Engine