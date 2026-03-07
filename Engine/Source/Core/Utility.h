//
// Created by ilya on 27.02.2026.
//

#ifndef ENGINE_UTILITY_H
#define ENGINE_UTILITY_H

#include <memory>

namespace Engine {

    // Aliases
    template<typename T>
    using Ref = std::shared_ptr<T>;

    // Using rule of five
    class NonCopyable {
        public:
        NonCopyable() = default;
        virtual ~NonCopyable() = default;

        // Delete copy operations
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;

        // Delete move operations
        NonCopyable(const NonCopyable&&) = delete;
        NonCopyable& operator=(const NonCopyable&&) = delete;
    };

}

#endif //ENGINE_UTILITY_H