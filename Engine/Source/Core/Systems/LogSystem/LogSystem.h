//
// Created by ilya on 27.02.2026.
//

#ifndef ENGINE_LOGGER_H
#define ENGINE_LOGGER_H

#include "Macro.h"
#include "Singleton.h"
#include <memory>
#include <concepts>
#include <string>
#include <string_view>

// Settings
#define ENGINE_LOG_TRACE_STATUS 1
#define ENGINE_LOG_DEBUG_STATUS 1
#define ENGINE_LOG_INFO_STATUS  1
#define ENGINE_LOG_WARN_STATUS  1
#define ENGINE_LOG_ERROR_STATUS 1
#define ENGINE_LOG_FATAL_STATUS 1

namespace Engine {

    // Severity enum
    enum class LogSeverityLevel : uint8_t {
        None = 0,
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Fatal,
    };

    // Log system
    class ENGINE_API LogSystem final : public Singleton<LogSystem> {
        friend class Singleton;
    public:
        // Interface for log
        void LogMessage(std::string_view categoryName, LogSeverityLevel severity, std::string_view message) const;

    private:
        LogSystem();
        ~LogSystem() override;

        // PIMPL idiom
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

    // concepts
    template<typename T>
    concept Loggable = std::convertible_to<T, std::string> || std::convertible_to<T, std::string_view>;

    // bounds
    constexpr auto minSeverityBound = LogSeverityLevel::Trace;
    constexpr auto maxSeverityBound = LogSeverityLevel::Fatal;

}

// Macro definitions

// LogSystem should be started in order to use Engine logs
#define ENGINE_LOG(categoryName, severity, ...)                                                                                                         \
do {                                                                                                                                                    \
    if constexpr(Engine::LogSeverityLevel::severity >= Engine::minSeverityBound && Engine::LogSeverityLevel::severity <= Engine::maxSeverityBound) {    \
        static_assert(Engine::Loggable<decltype(categoryName)>, "Unable to log category because of unconvertible type!");                               \
        static_assert(Engine::Loggable<decltype(__VA_ARGS__)>, "Unable to log message because of unconvertible type!");                                 \
        std::string formattedStr = std::format(#__VA_ARGS__);                                                                                           \
        Engine::LogSystem::GetInstance()->LogMessage(categoryName, Engine::LogSeverityLevel::severity, formattedStr);                                   \
    }                                                                                                                                                   \
} while (0)

// Log trace
#if ENGINE_LOG_TRACE_STATUS == 1
    #define ENGINE_LOG_TRACE(categoryName, ...) ENGINE_LOG(categoryName, Trace, __VA_ARGS__)
#else
    #define ENGINE_LOG_TRACE(...)
#endif

// Log debug
#if ENGINE_LOG_DEBUG_STATUS == 1
    #define ENGINE_LOG_DEBUG(categoryName, ...) ENGINE_LOG(categoryName, Debug, __VA_ARGS__)
#else
    #define ENGINE_LOG_DEBUG(...)
#endif

// Log info
#if ENGINE_LOG_INFO_STATUS == 1
    #define ENGINE_LOG_INFO(categoryName, ...) ENGINE_LOG(categoryName, Info, __VA_ARGS__)
#else
    #define ENGINE_LOG_INFO(...)
#endif

// Log warn
#if ENGINE_LOG_WARN_STATUS == 1
    #define ENGINE_LOG_WARN(categoryName, ...) ENGINE_LOG(categoryName, Warn, __VA_ARGS__)
#else
    #define ENGINE_LOG_WARN(...)
#endif

// Log error
#if ENGINE_LOG_ERROR_STATUS == 1
    #define ENGINE_LOG_ERROR(categoryName, ...) ENGINE_LOG(categoryName, Error, __VA_ARGS__)
#else
    #define ENGINE_LOG_ERROR(...)
#endif

// Log fatal
#if ENGINE_LOG_FATAL_STATUS == 1
    #define ENGINE_LOG_FATAL(categoryName, ...) ENGINE_LOG(categoryName, Fatal, __VA_ARGS__)
#else
    #define ENGINE_LOG_FATAL(...)
#endif

#endif //ENGINE_LOGGER_H