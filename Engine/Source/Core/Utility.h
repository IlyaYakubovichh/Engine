//
// Created by ilya on 27.02.2026.
//

#ifndef ENGINE_UTILITY_H
#define ENGINE_UTILITY_H

namespace Engine {

    // Using rule of five
    class NonCopyable {
        public:
        NonCopyable() = default;
        ~NonCopyable() = default;

        // Delete copy operations
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;

        // Delete move operations
        NonCopyable(const NonCopyable&&) = delete;
        NonCopyable& operator=(const NonCopyable&&) = delete;
    };

}

#endif //ENGINE_UTILITY_H