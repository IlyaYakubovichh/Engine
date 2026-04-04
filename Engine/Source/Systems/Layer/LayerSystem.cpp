#include "Macros.h"
#include "Layer/LayerSystem.h"
#include "Log/LogSystem.h"

namespace Engine {

    // ─── PIMPL ───────────────────────────────────────────────────────────────────

    class LayerSystem::Impl {
    public:
        Impl() = default;

        ~Impl()
        {
            for (Layer* layer : mLayers) {
                delete layer;
            }
            ENGINE_LOG_DEBUG("LayerSystem", "All layers destroyed");
        }

        void PushLayer(Layer* layer)
        {
            ENGINE_ASSERT_MSG(layer, "PushLayer: null layer");
            mLayers.emplace(mLayers.begin() + mLayerInsertIndex, layer);
            ++mLayerInsertIndex;
            ENGINE_LOG_DEBUG("LayerSystem", "Layer '{}' pushed (index: {})", layer->GetName(), mLayerInsertIndex - 1);
        }

        void PushOverlay(Layer* overlay)
        {
            ENGINE_ASSERT_MSG(overlay, "PushOverlay: null overlay");
            mLayers.emplace_back(overlay);
            ENGINE_LOG_DEBUG("LayerSystem", "Overlay '{}' pushed", overlay->GetName());
        }

        void PopLayer(Layer* layer)
        {
            ENGINE_ASSERT_MSG(layer, "PopLayer: null layer");

            const auto it = std::ranges::find(mLayers, layer);
            if (it == mLayers.end()) {
                ENGINE_LOG_WARN("LayerSystem", "PopLayer: '{}' not found", layer->GetName());
                return;
            }

            mLayers.erase(it);
            --mLayerInsertIndex;
            ENGINE_LOG_DEBUG("LayerSystem", "Layer '{}' popped", layer->GetName());
        }

        void PopOverlay(Layer* overlay)
        {
            ENGINE_ASSERT_MSG(overlay, "PopOverlay: null overlay");

            const auto it = std::ranges::find(mLayers, overlay);
            if (it == mLayers.end()) {
                ENGINE_LOG_WARN("LayerSystem", "PopOverlay: '{}' not found", overlay->GetName());
                return;
            }

            mLayers.erase(it);
            ENGINE_LOG_DEBUG("LayerSystem", "Overlay '{}' popped", overlay->GetName());
        }

        [[nodiscard]] const Layer* GetLayerByName(const std::string& name) const
        {
            for (const Layer* layer : mLayers) {
                if (layer->GetName() == name) return layer;
            }
            ENGINE_LOG_WARN("LayerSystem", "GetLayerByName: '{}' not found", name);
            return nullptr;
        }

        [[nodiscard]] const std::vector<Layer*>& GetLayers() const { return mLayers; }

    private:
        std::vector<Layer*> mLayers;
        uint32_t            mLayerInsertIndex{ 0 };
    };

    // ─── LayerSystem shell ────────────────────────────────────────────────────────

    LayerSystem::LayerSystem() : pImpl(std::make_unique<Impl>()) {}
    LayerSystem::~LayerSystem() = default;

    void         LayerSystem::PushLayer(Layer* layer)                       { pImpl->PushLayer(layer);              }
    void         LayerSystem::PushOverlay(Layer* overlay)                   { pImpl->PushOverlay(overlay);          }
    void         LayerSystem::PopLayer(Layer* layer)                        { pImpl->PopLayer(layer);               }
    void         LayerSystem::PopOverlay(Layer* overlay)                    { pImpl->PopOverlay(overlay);           }
    const Layer* LayerSystem::GetLayerByName(const std::string& name) const { return pImpl->GetLayerByName(name);   }
    const std::vector<Layer*>& LayerSystem::GetLayers() const               { return pImpl->GetLayers();            }

} // namespace Engine