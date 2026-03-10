//
// Created by ilya on 10.03.2026.
//

#ifndef ENGINE_SUBSYSTEM_H
#define ENGINE_SUBSYSTEM_H

#include "Utility.h"

namespace Engine {

    class ENGINE_API Subsystem : public NonCopyable {
    public:
        Subsystem()           = default;
        ~Subsystem() override = default;

        virtual void Destroy() = 0;

        [[nodiscard]] bool IsInitialized() const { return mInitialized; }

    protected:
        // Concrete subsystems call this inside their Initialize() once setup succeeds.
        void MarkInitialized()   { mInitialized = true;  }
        void MarkUninitialized() { mInitialized = false; }

    private:
        bool mInitialized { false };
    };

} // namespace Engine

#endif //ENGINE_SUBSYSTEM_H