//
// Created by ilya on 26.02.2026.
//

#ifndef ENGINE_MACRO_H
#define ENGINE_MACRO_H

// .dll export/import macro
#ifdef ENGINE_EXPORTS
    #define ENGINE_API __declspec(dllexport)
#else
    #define ENGINE_API __declspec(dllimport)
#endif

// Platform detection for Debug Break
#if defined(_WIN32)
    #define ENGINE_DEBUGBREAK() __debugbreak()
#elif defined(__linux__) || defined(__APPLE__)
    #define ENGINE_DEBUGBREAK() __builtin_trap()
#else
    #define ENGINE_DEBUGBREAK() ((void)0)
#endif

// Enable asserts only in Debug builds
#if defined(ENGINE_DEBUG)
    #define ENGINE_ENABLE_ASSERTS
#endif

// Assert definitions
#ifdef ENGINE_ENABLE_ASSERTS

    #define ENGINE_ASSERT(x)                                                                            \
        do {                                                                                            \
            if (!(x)) {                                                                                 \
                ENGINE_LOG_FATAL("Assertion", "Failed: " #x " in " __FILE__ " (" << __LINE__ << ")");   \
                ENGINE_DEBUGBREAK();                                                                    \
            }                                                                                           \
        } while(0)

    #define ENGINE_ASSERT_MESSAGE(x, msg)           \
    do {                                            \
        if(!(x)) {                                  \
            ENGINE_LOG_FATAL("Assertion", msg);     \
            ENGINE_DEBUGBREAK();                    \
        }                                           \
    } while(0)

    #define ENGINE_STATIC_ASSERT(expr) \
    static_assert(expr, "Static assertion failed: " #expr)

#else
    // Strip asserts in Release builds
    #define ENGINE_ASSERT(x)
    #define ENGINE_ASSERT_MESSAGE(x, msg)
#endif

#define LEFT_SHIFT(value) ((value) << 1)
#define RIGHT_SHIFT(value) ((value) >> 1)

#endif //ENGINE_MACRO_H