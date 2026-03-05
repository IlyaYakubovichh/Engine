#ifndef ENGINE_LAYERSYSTEM_H
#define ENGINE_LAYERSYSTEM_H

#include "Layer.h"
#include "Singleton.h"
#include <memory>
#include <string>
#include <vector>

namespace Engine {

    class ENGINE_API LayerSystem final : public Singleton<LayerSystem> {
        friend class Singleton;

    public:
        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void PopLayer(const Layer* layer);
        void PopOverlay(const Layer* overlay);

        [[nodiscard]] const Layer*              GetLayerByName(const std::string& name) const;
        [[nodiscard]] const std::vector<Layer*>& GetLayers()                            const;

    private:
        LayerSystem();
        ~LayerSystem() override;

        // PIMPL
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace Engine

#endif // ENGINE_LAYERSYSTEM_H