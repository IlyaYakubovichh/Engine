#include "LayerSystem.h"
#include "Macro.h"
#include "Systems/LogSystem/LogSystem.h"

namespace Engine {

// ---------------------------------------------------------------------------
// PIMPL
// ---------------------------------------------------------------------------
    class LayerSystem::Impl {
    public:
        Impl()  = default;

        ~Impl() {
            for (const Layer* layer : mLayers) {
                delete layer;
            }
            mLayers.clear();
            ENGINE_LOG_INFO("LayerSystem", "All layers destroyed");
        }

        void PushLayer(Layer* layer) {
            ENGINE_ASSERT_MESSAGE(layer, "Attempted to PushLayer(nullptr)");

            mLayers.emplace(mLayers.begin() + mLayerInsertIndex, layer);
            mLayerInsertIndex++;

            ENGINE_LOG_INFO("LayerSystem", "Layer '{}' pushed (index: {})", layer->GetName(), mLayerInsertIndex - 1);
        }

        void PushOverlay(Layer* overlay) {
            ENGINE_ASSERT_MESSAGE(overlay, "Attempted to PushOverlay(nullptr)");

            mLayers.emplace_back(overlay);

            ENGINE_LOG_INFO("LayerSystem", "Overlay '{}' pushed", overlay->GetName());
        }

        void PopLayer(const Layer* layer) {
            ENGINE_ASSERT_MESSAGE(layer, "Attempted to PopLayer(nullptr)");

            if (const auto it = std::ranges::find(mLayers, layer); it != mLayers.end()) {
                ENGINE_LOG_INFO("LayerSystem", "Layer '{}' popped", layer->GetName());
                mLayers.erase(it);
                mLayerInsertIndex--;
            } else {
                ENGINE_LOG_WARN("LayerSystem", "PopLayer: layer '{}' not found", layer->GetName());
            }
        }

        void PopOverlay(const Layer* overlay) {
            ENGINE_ASSERT_MESSAGE(overlay, "Attempted to PopOverlay(nullptr)");

            if (const auto it = std::ranges::find(mLayers, overlay); it != mLayers.end()) {
                ENGINE_LOG_INFO("LayerSystem", "Overlay '{}' popped", overlay->GetName());
                mLayers.erase(it);
            } else {
                ENGINE_LOG_WARN("LayerSystem", "PopOverlay: overlay '{}' not found", overlay->GetName());
            }
        }

        [[nodiscard]] const Layer* GetLayerByName(const std::string& name) const {
            for (const Layer* layer : mLayers) {
                if (layer->GetName() == name) return layer;
            }
            ENGINE_LOG_WARN("LayerSystem", "GetLayerByName: '{}' not found", name);
            return nullptr;
        }

        [[nodiscard]] const std::vector<Layer*>& GetLayers() const {
            return mLayers;
        }

    private:
        std::vector<Layer*> mLayers;
        uint32_t            mLayerInsertIndex { 0 };
    };

// ---------------------------------------------------------------------------
// LayerSystem - thin shell
// ---------------------------------------------------------------------------
    LayerSystem::LayerSystem()
        : pImpl(std::make_unique<Impl>()) {}

    LayerSystem::~LayerSystem() = default;

    void LayerSystem::PushLayer(Layer* layer)          { pImpl->PushLayer(layer);       }
    void LayerSystem::PushOverlay(Layer* overlay)      { pImpl->PushOverlay(overlay);   }
    void LayerSystem::PopLayer(const Layer* layer)     { pImpl->PopLayer(layer);         }
    void LayerSystem::PopOverlay(const Layer* overlay) { pImpl->PopOverlay(overlay);     }

    const Layer* LayerSystem::GetLayerByName(const std::string& name) const {
        return pImpl->GetLayerByName(name);
    }

    const std::vector<Layer*>& LayerSystem::GetLayers() const {
        return pImpl->GetLayers();
    }

} // namespace Engine