#pragma once
#include "Macros.h"
#include "Layer.h"
#include "Singleton.h"
#include "Utils.h"
#include <memory>
#include <string>
#include <vector>

namespace Engine {

    /**
     * @brief Manages the ordered stack of engine layers and overlays.
     *
     * Layers occupy the lower half of the stack (insertion-ordered),
     * overlays always sit on top.
     */
    class ENGINE_API LayerSystem final : public Singleton<LayerSystem> {
        friend class Singleton;
    public:
        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* overlay);

        // Returns the first layer matching the given name, or nullptr.
        [[nodiscard]] const Layer* GetLayerByName(const std::string& name) const;

        // Returns the full ordered layer list (layers first, overlays last).
        [[nodiscard]] const std::vector<Layer*>& GetLayers() const;

    private:
        LayerSystem();
        ~LayerSystem() override;

        class Impl;
        Scope<Impl> pImpl;
    };

} // namespace Engine