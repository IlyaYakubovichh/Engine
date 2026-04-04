#pragma once
#include "Macros.h"
#include "Utils.h"
#include "Singleton.h"
#include <memory>
#include <string_view>
#include <format>

// ─── Per-severity compile-time switches ──────────────────────────────────────
#define ENGINE_LOG_TRACE_STATUS 1

#ifdef ENGINE_DEBUG
#define ENGINE_LOG_DEBUG_STATUS 1
#else
#define ENGINE_LOG_DEBUG_STATUS 0
#endif

#define ENGINE_LOG_INFO_STATUS  1
#define ENGINE_LOG_WARN_STATUS  1
#define ENGINE_LOG_ERROR_STATUS 1
#define ENGINE_LOG_FATAL_STATUS 1

namespace Engine {

    /// Log severity levels, ordered lowest to highest.
    enum class LogSeverityLevel : uint8_t {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Fatal,
    };

    inline constexpr LogSeverityLevel cMinSeverity = LogSeverityLevel::Trace;
    inline constexpr LogSeverityLevel cMaxSeverity = LogSeverityLevel::Fatal;

    /**
     * @brief Singleton log system backed by spdlog.
     *
     * Use the ENGINE_LOG_* macros — do not call LogMessage directly.
     */
    class ENGINE_API LogSystem final : public Singleton<LogSystem> {
        friend class Singleton;
    public:
        void LogMessage(
            std::string_view category,
            LogSeverityLevel severity,
            std::string_view message) const;

    private:
        LogSystem();
        ~LogSystem() override;

        class Impl;
        Scope<Impl> pImpl;
    };

} // namespace Engine

// ─── Core log macro ───────────────────────────────────────────────────────────
#define ENGINE_LOG(category, severity, ...)                                         \
    do {                                                                            \
        if constexpr (Engine::LogSeverityLevel::severity >= Engine::cMinSeverity && \
                      Engine::LogSeverityLevel::severity <= Engine::cMaxSeverity) { \
            Engine::LogSystem::GetInstance()->LogMessage(                           \
                category,                                                           \
                Engine::LogSeverityLevel::severity,                                 \
                std::format(__VA_ARGS__));                                          \
        }                                                                           \
    } while (0)

// ─── Convenience macros ───────────────────────────────────────────────────────
#if ENGINE_LOG_TRACE_STATUS
#define ENGINE_LOG_TRACE(cat, ...) ENGINE_LOG(cat, Trace, __VA_ARGS__)
#else
#define ENGINE_LOG_TRACE(cat, ...) ((void)0)
#endif

#if ENGINE_LOG_DEBUG_STATUS
#define ENGINE_LOG_DEBUG(cat, ...) ENGINE_LOG(cat, Debug, __VA_ARGS__)
#else
#define ENGINE_LOG_DEBUG(cat, ...) ((void)0)
#endif

#if ENGINE_LOG_INFO_STATUS
#define ENGINE_LOG_INFO(cat, ...)  ENGINE_LOG(cat, Info,  __VA_ARGS__)
#else
#define ENGINE_LOG_INFO(cat, ...)  ((void)0)
#endif

#if ENGINE_LOG_WARN_STATUS
#define ENGINE_LOG_WARN(cat, ...)  ENGINE_LOG(cat, Warn,  __VA_ARGS__)
#else
#define ENGINE_LOG_WARN(cat, ...)  ((void)0)
#endif

#if ENGINE_LOG_ERROR_STATUS
#define ENGINE_LOG_ERROR(cat, ...) ENGINE_LOG(cat, Error, __VA_ARGS__)
#else
#define ENGINE_LOG_ERROR(cat, ...) ((void)0)
#endif

#if ENGINE_LOG_FATAL_STATUS
#define ENGINE_LOG_FATAL(cat, ...) ENGINE_LOG(cat, Fatal, __VA_ARGS__)
#else
#define ENGINE_LOG_FATAL(cat, ...) ((void)0)
#endif