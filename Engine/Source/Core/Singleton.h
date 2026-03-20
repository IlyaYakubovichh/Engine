#pragma once

#include "Utils.h"

namespace Engine {

    /**
     * @brief CRTP-ready singleton base.
     *
     * Provides a single globally accessible instance of T.
     * Start() allocates, Shutdown() deallocates. Not thread-safe by design —
     * all systems are initialised sequentially from Application::Start().
     *
     * @tparam T Concrete system type that inherits Singleton<T>.
     */
    template<typename T>
    class Singleton : public NonCopyable {
    public:
        // Constructs the instance. No-op if already alive.
        template<typename... Args>
        static void Start(Args&&... args)
        {
            if (!sInstance) {
                sInstance = new T(std::forward<Args>(args)...);
            }
        }

        // Destroys the instance. No-op if already null.
        static void Shutdown()
        {
            delete sInstance;
            sInstance = nullptr;
        }

        // Returns a raw pointer to the live instance.
        [[nodiscard]] static T* GetInstance() { return sInstance; }

    protected:
        Singleton() = default;
        ~Singleton() = default;

    private:
        inline static T* sInstance = nullptr;
    };

} // namespace Engine