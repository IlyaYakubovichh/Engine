#pragma once

#include "Macros.h"
#include "Utils.h"

namespace Engine {

    /**
     * @brief Base class for all Vulkan subsystems.
     *
     * Tracks initialization state. Concrete subsystems call
     * MarkInitialized() / MarkUninitialized() inside their own
     * Initialize() / Destroy() implementations.
     */
    class ENGINE_API VulkanSubsystem : public NonCopyable {
    public:
        VulkanSubsystem() = default;
        ~VulkanSubsystem() override = default;

        virtual void Destroy() = 0;

        [[nodiscard]] bool IsInitialized() const { return mInitialized; }

    protected:
        void MarkInitialized() { mInitialized = true; }
        void MarkUninitialized() { mInitialized = false; }

    private:
        bool mInitialized{ false };
    };

} // namespace Engine