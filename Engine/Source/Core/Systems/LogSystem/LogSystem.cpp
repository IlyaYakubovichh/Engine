//
// Created by ilya on 27.02.2026.
//

#include "LogSystem.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Engine {

    // Anonymous namespace - internal linkage
    namespace {
        constexpr auto clogPattern = "[%H:%M:%S.%e] [%^%l%$] %v";

        spdlog::level::level_enum ConvertSeverityToSpdlog(const LogSeverityLevel severity) {
            switch (severity) {
                case LogSeverityLevel::Trace:   return spdlog::level::trace;
                case LogSeverityLevel::Debug:   return spdlog::level::debug;
                case LogSeverityLevel::Info:    return spdlog::level::info;
                case LogSeverityLevel::Warn:    return spdlog::level::warn;
                case LogSeverityLevel::Error:   return spdlog::level::err;
                case LogSeverityLevel::Fatal:   return spdlog::level::critical;
                default:                        return spdlog::level::off;
            }
        }
    }

    // PIMPL
    class LogSystem::Impl {
    public:
        Impl() {
            // Create console sink
            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            consoleSink->set_pattern(clogPattern);

            // Create Console logger
            mConsoleLogger = std::make_shared<spdlog::logger>("Engine logger", consoleSink);
            mConsoleLogger->set_pattern(clogPattern);
            mConsoleLogger->set_level(spdlog::level::trace);
        }

        ~Impl() {
            if (mConsoleLogger) {
                mConsoleLogger->flush();
            }
            mConsoleLogger.reset();
        }

        // Logger
        void LogMessage(const std::string_view categoryName, const LogSeverityLevel severity, const std::string_view message) const {
            if (!mConsoleLogger) return;

            const spdlog::level::level_enum level = ConvertSeverityToSpdlog(severity);

            // Format: [Category] Message
            // Using standard string concatenation
            const std::string formattedMessage = std::string("[")
                                                .append(categoryName)
                                                .append("] ")
                                                .append(message);

            mConsoleLogger->log(level, formattedMessage);

            if (level == spdlog::level::critical) ENGINE_DEBUGBREAK();
        }

    private:
        std::shared_ptr<spdlog::logger> mConsoleLogger;
    };

    // Interface for PIMPL
    void LogSystem::LogMessage(const std::string_view categoryName, const LogSeverityLevel severity, const std::string_view message) const {
        if (pImpl) {
            pImpl->LogMessage(categoryName, severity, message);
        }
    }

    LogSystem::LogSystem()
        : pImpl(std::make_unique<Impl>()) {
    }

    LogSystem::~LogSystem() = default;

}
