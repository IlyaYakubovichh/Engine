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

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent() {}

        [[nodiscard]] const std::string& GetName() const { return mName; }

    private:
        std::string mName { "Undefined" };
    };

}

#endif //ENGINE_LAYER_H