//
// Created by ilya on 28.02.2026.
//

#ifndef ENGINE_LAYERSYSTEM_H
#define ENGINE_LAYERSYSTEM_H

#include "Layer.h"
#include "Singleton.h"
#include <cstdint>
#include <vector>

namespace Engine {

    class ENGINE_API LayerSystem final : public Singleton<LayerSystem> {
        friend class Singleton;
    public:
        // Interface for add-remove
        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void PopLayer(const Layer* layer);
        void PopOverlay(const Layer* overlay);

        // Accessors
        [[nodiscard]] const Layer* GetLayerByName(const std::string& name) const;
        [[nodiscard]] const std::vector<Layer*>& GetLayers() const;

    private:
        LayerSystem() = default;
        ~LayerSystem() override;

        std::vector<Layer*> mLayers;
        std::uint32_t mLayerInsertIndex{ 0 };
    };

}

#endif //ENGINE_LAYERSYSTEM_H