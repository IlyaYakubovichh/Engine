//
// Created by ilya on 28.02.2026.
//

#ifndef ENGINE_LAYER_H
#define ENGINE_LAYER_H

#include "Macro.h"
#include "Utility.h"
#include <string>

namespace Engine {

    class ENGINE_API Layer : public NonCopyable {
    public:
        explicit Layer(std::string  name);
        ~Layer() override;

        virtual void OnAttach() const {}
        virtual void OnDetach() const {}
        virtual void OnUpdate() const {}
        virtual void OnEvent() const {}

        [[nodiscard]] const std::string& GetName() const { return mName; }

    private:
        std::string mName { "Undefined" };
    };

}

#endif //ENGINE_LAYER_H