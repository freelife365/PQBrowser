#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/ostream_sink.h>

namespace MBase
{

/// available log levels (default: INFO), corresponding to those of spdlog
enum class Log_Level
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERR,
    CRITICAL,
    OFF
};

class Logger
{
public:
    /// set the log level for all loggers
    static void setLevel(Log_Level level)
    {
        getInstance().m_level = level;

        // set level globally (probably superfluous..)
        spdlog::set_level(convertLevelToSpdlog(level));

        // set level for all active loggers
        for (auto& [key, logger] : getInstance().m_loggers)
        {
            logger->set_level(convertLevelToSpdlog(level));
        }
    }

    /// enable file logging
    static void enableFileLogging(std::string filename)
    {
        getInstance().m_fileName = std::move(filename);
    }

    /// flush all loggers
    static void flush()
    {
        for (auto& [key, logger] : getInstance().m_loggers)
        {
            for (auto& sink : logger->sinks())
            {
                sink->flush();
            }
        }
    }

    /// add a sink that writes to `std::ostream` (useful for testing)
    static void addSink(std::ostream& os)
    {
        sinks()->add_sink(std::make_shared<spdlog::sinks::ostream_sink_st>(os));
    }

    /// convert LogLevel to spdlog::level::level_enum
    static spdlog::level::level_enum convertLevelToSpdlog(Log_Level level)
    {
        switch (level)
        {
            case Log_Level::TRACE: return spdlog::level::trace;
            case Log_Level::DEBUG: return spdlog::level::debug;
            case Log_Level::INFO: return spdlog::level::info;
            case Log_Level::WARN: return spdlog::level::warn;
            case Log_Level::ERR: return spdlog::level::err;
            case Log_Level::CRITICAL: return spdlog::level::critical;
            case Log_Level::OFF: return spdlog::level::off;
            default: return spdlog::level::info;
        }
    }

    /// return a logger corresponding to "name" (if not existing, it is created)
    static std::shared_ptr<spdlog::logger> get(std::string name)
    {
        // If we don't have sinks setup yet, initialize it
        if (getInstance().m_loggers.count(name) == 0)
        {
            auto newLogger = std::make_shared<spdlog::logger>(name, sinks());
            newLogger->set_level(convertLevelToSpdlog(getInstance().m_level));

            //����info���������ϵ�����ˢ��
            newLogger->flush_on(spdlog::level::info);

            //ÿ����ˢ��һ��
            //spdlog::flush_every(std::chrono::seconds(3));

            getInstance().m_loggers[name] = newLogger;
        }

        return getInstance().m_loggers[name];
    }

private:
    /// returns the singleton instance of Logger
    static Logger& getInstance()
    {
        static Logger inst;
        return inst;
    }

    /// Static function initialising the sinks
    static std::shared_ptr<spdlog::sinks::dist_sink_mt> initSinks()
    {
        auto sink = std::make_shared<spdlog::sinks::dist_sink_mt>();

#if _DEBUG
        // Add a console output sink
        sink->add_sink(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#endif // _DEBUG

        // If a filename is set, add a file output sink
        if (getInstance().m_fileName != "")
        {
            //sink->add_sink(std::make_shared<spdlog::sinks::daily_file_sink_mt>(getInstance().m_fileName, 23, 59));
            sink->add_sink(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                getInstance().m_fileName, 1048576 * 5, 3));
        }

        return sink;
    }

    /// Return the sinks stored for the loggers
    static std::shared_ptr<spdlog::sinks::dist_sink_mt> sinks()
    {
        static auto sink = initSinks();
        return sink;
    }

    /// the log level
    Log_Level m_level{Log_Level::INFO};

    /// the file name for file logging (if enabled)
    std::string m_fileName;

    /// map storing the loggers identified by a string
    std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> m_loggers;
};

template <class Duration = std::chrono::milliseconds, class Clock = std::chrono::steady_clock>
class TimerLog
{
public:
    TimerLog(const std::string& message)
        : m_start{Clock::now()}
        , m_message{message}
    {
    }

    ~TimerLog()
    {
        typename Clock::time_point stop = Clock::now();
        auto timeElapsed =
            std::to_string(std::chrono::duration_cast<Duration>(stop - m_start).count());
        Logger::get("Timing")->info("Execution of {}: took {} ms", m_message, timeElapsed);
    }

private:
    TimerLog(const TimerLog&) = delete;
    TimerLog& operator=(const TimerLog&) = delete;
    TimerLog(TimerLog&&) = delete;
    TimerLog& operator=(TimerLog&&) = delete;

private:
    typename Clock::time_point m_start;
    std::string m_message; /// the message to output (in addition to timing)
};
} // namespace MBase

#define LOGGER_TRACE(x, ...) SPDLOG_LOGGER_TRACE(MBase::Logger::get(x), __VA_ARGS__)
#define LOGGER_DEBUG(x, ...) SPDLOG_LOGGER_DEBUG(MBase::Logger::get(x), __VA_ARGS__)
#define LOGGER_INFO(x, ...)  SPDLOG_LOGGER_INFO(MBase::Logger::get(x), __VA_ARGS__)
#define LOGGER_WARN(x, ...)  SPDLOG_LOGGER_WARN(MBase::Logger::get(x), __VA_ARGS__)
#define LOGGER_ERROR(x, ...) SPDLOG_LOGGER_ERROR(MBase::Logger::get(x), __VA_ARGS__)
#define LOGGER_FATAL(x, ...) SPDLOG_LOGGER_CRITICAL(MBase::Logger::get(x), __VA_ARGS__)
