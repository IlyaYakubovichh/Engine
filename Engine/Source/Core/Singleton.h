//
// Created by ilya on 27.02.2026.
//

#ifndef ENGINE_SINGLETON_H
#define ENGINE_SINGLETON_H

#include "Macro.h"
#include "Utility.h"

namespace Engine {

    // Singleton (class with ONLY one instance)
    template<typename T>
    class ENGINE_API Singleton : public NonCopyable {
    public:
        // Creates an instance if not present
        template<typename... Args>
        static void Start(Args... args) {
            if (!sInstance) {
                sInstance = new T(args...);
            }
        }

        // If called on nullptr, then this method has no effect
        static void Shutdown() {
            delete sInstance;
        }

        // Get an instance
        static T* GetInstance() {
            return sInstance;
        }

    protected:
        Singleton() = default;
        ~Singleton() = default;

        inline static T* sInstance = nullptr; // C++17 inline static variable
    };

}

#endif //ENGINE_SINGLETON_H