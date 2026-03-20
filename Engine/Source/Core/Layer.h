#pragma once

#include "Macros.h"
#include "Utils.h"
#include <string>

namespace Engine {

    /**
     * @brief Abstract base for all engine layers.
     *
     * Layers are pushed onto the LayerStack and receive lifecycle
     * callbacks (Attach / Detach / Update / Event) each frame.
     */
    class ENGINE_API Layer : public NonCopyable {
    public:
        explicit Layer(std::string name);
        ~Layer() override = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate() {}
        virtual void OnEvent() {}

        // Returns the debug name assigned at construction.
        [[nodiscard]] const std::string& GetName() const { return mName; }

    private:
        std::string mName;
    };

} // namespace Engine