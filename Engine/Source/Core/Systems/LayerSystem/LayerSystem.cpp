//
// Created by ilya on 28.02.2026.
//

#include "Macro.h"
#include "LayerSystem.h"
#include "Systems/LogSystem/LogSystem.h"

namespace Engine {

    LayerSystem::~LayerSystem() {
        for (const Layer* layer : mLayers) {
            delete layer;
        }

        mLayers.clear();
    }

    void LayerSystem::PushLayer(Layer* layer) {
        ENGINE_ASSERT_MESSAGE(layer, "Attempted to PushLayer(nullptr)");

        // Insert layer at the correct index before overlays
        mLayers.emplace(mLayers.begin() + mLayerInsertIndex, layer);
        mLayerInsertIndex++;
    }

    void LayerSystem::PushOverlay(Layer* overlay) {
        ENGINE_ASSERT_MESSAGE(overlay, "Attempted to PushOverlay(nullptr)");

        // Overlays are always pushed to the back
        mLayers.emplace_back(overlay);
    }

    void LayerSystem::PopLayer(const Layer* layer) {
        ENGINE_ASSERT_MESSAGE(layer, "Attempted to PopLayer(nullptr)");

        if (const auto it = std::ranges::find(mLayers, layer); it != mLayers.end()) {
            mLayers.erase(it);

            // If we removed a regular layer, we must decrement the insert index
            // to keep the boundary between Layers and Overlays correct.
            mLayerInsertIndex--;
        }
        else {
            ENGINE_LOG_WARN("Layer Stack", "Could not pop layer: Layer not found");
        }
    }

    void LayerSystem::PopOverlay(const Layer* overlay) {
        ENGINE_ASSERT_MESSAGE(overlay, "Attempted to PopOverlay(nullptr)");

        if (const auto it = std::ranges::find(mLayers, overlay); it != mLayers.end()) {
            mLayers.erase(it);
        }
        else {
            ENGINE_LOG_WARN("Layer Stack", "Could not pop overlay: Overlay not found");
        }
    }

    const Layer* LayerSystem::GetLayerByName(const std::string& name) const {
        for (const Layer* layer : mLayers) {
            if (layer->GetName() == name) {
                return layer;
            }
        }

        ENGINE_LOG_WARN("LayerSystem", "GetLayerByName: Layer not found, nullptr returned!");
        return nullptr;
    }

    const std::vector<Layer*>& LayerSystem::GetLayers() const {
        return mLayers;
    }

}