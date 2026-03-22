#pragma once

// ─── DLL export / import ─────────────────────────────────────────────────────
#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

// ─── Debug break ─────────────────────────────────────────────────────────────
#if defined(_WIN32)
#define ENGINE_DEBUGBREAK() __debugbreak()
#elif defined(__linux__) || defined(__APPLE__)
#define ENGINE_DEBUGBREAK() __builtin_trap()
#else
#define ENGINE_DEBUGBREAK() ((void)0)
#endif

// ─── Asserts (stripped in Release) ───────────────────────────────────────────
#if defined(ENGINE_DEBUG)
#define ENGINE_ASSERT(x)                                                            \
        do {                                                                        \
            if (!(x)) {                                                             \
                ENGINE_LOG_FATAL("Assert", "Failed: " #x                            \
                    " | " __FILE__ " (" << __LINE__ << ")");                        \
                ENGINE_DEBUGBREAK();                                                \
            }                                                                       \
        } while (0)

#define ENGINE_ASSERT_MSG(x, msg)                                                   \
        do {                                                                        \
            if (!(x)) {                                                             \
                ENGINE_LOG_FATAL("Assert", #msg);                                   \
                ENGINE_DEBUGBREAK();                                                \
            }                                                                       \
        } while (0)

#define ENGINE_STATIC_ASSERT(x) static_assert(x, "Static assert failed: " #x)
#else
#define ENGINE_ASSERT(x)          ((void)0)
#define ENGINE_ASSERT_MSG(x, msg) ((void)0)
#define ENGINE_STATIC_ASSERT(x)   static_assert(x, "Static assert failed: " #x)
#endif

// ─── Bit helpers ──────────────────────────────────────────────────────────────
#define BIT_LEFT(n)  (1u << (n))
#define BIT_RIGHT(n) (1u >> (n))