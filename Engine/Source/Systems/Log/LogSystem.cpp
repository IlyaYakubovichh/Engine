#include "Log/LogSystem.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Engine {

    namespace {

        constexpr auto cLogPattern = "[%H:%M:%S.%e] [%^%l%$] %v";

        spdlog::level::level_enum ToSpdlogLevel(const LogSeverityLevel severity)
        {
            switch (severity) {
            case LogSeverityLevel::Trace: return spdlog::level::trace;
            case LogSeverityLevel::Debug: return spdlog::level::debug;
            case LogSeverityLevel::Info:  return spdlog::level::info;
            case LogSeverityLevel::Warn:  return spdlog::level::warn;
            case LogSeverityLevel::Error: return spdlog::level::err;
            case LogSeverityLevel::Fatal: return spdlog::level::critical;
            default:                      return spdlog::level::off;
            }
        }

    } // namespace

    // ─── PIMPL ───────────────────────────────────────────────────────────────────

    class LogSystem::Impl {
    public:
        Impl()
        {
            auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sink->set_pattern(cLogPattern);

            mLogger = std::make_shared<spdlog::logger>("Engine", sink);
            mLogger->set_level(spdlog::level::trace);
        }

        ~Impl()
        {
            if (mLogger) mLogger->flush();
        }

        void LogMessage(std::string_view category,
            LogSeverityLevel  severity,
            std::string_view  message) const
        {
            if (!mLogger) return;

            const auto level = ToSpdlogLevel(severity);

            // Format: [Category] Message
            mLogger->log(level,
                std::string("[").append(category).append("] ").append(message));

            if (level == spdlog::level::critical) ENGINE_DEBUGBREAK();
        }

    private:
        std::shared_ptr<spdlog::logger> mLogger;
    };

    // ─── LogSystem shell ──────────────────────────────────────────────────────────

    LogSystem::LogSystem() : pImpl(std::make_unique<Impl>()) {}
    LogSystem::~LogSystem() = default;

    void LogSystem::LogMessage(std::string_view category,
        LogSeverityLevel  severity,
        std::string_view  message) const
    {
        if (pImpl) pImpl->LogMessage(category, severity, message);
    }

} // namespace Engine