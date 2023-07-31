#include "logger.hpp"

#include <array>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <time.h>

namespace mud
{
    // todo
    // filter by category?
    // filter by log level?

    auto getFormattedTimeStamp()
    {
        std::time_t clockTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        return std::put_time(std::localtime(&clockTime), "[%X]");
    }

    const char * getColorCode(LogColor color)
    {
        static const std::array<const char *, 19> logColorCodeStrings{
            "\x1b[0m",    // Default settings
            "\x1b[30m",   // Black
            "\x1b[31m",   // Red
            "\x1b[32m",   // Green
            "\x1b[33m",   // Yellow
            "\x1b[34m",   // Blue
            "\x1b[35m",   // Magenta
            "\x1b[36m",   // Cyan
            "\x1b[37m",   // White
            "\x1b[90m",   // Bright Black
            "\x1b[91m",   // Bright Red
            "\x1b[92m",   // Bright Green
            "\x1b[93m",   // Bright Yellow
            "\x1b[94m",   // Bright Blue
            "\x1b[95m",   // Bright Magenta
            "\x1b[96m",   // Bright Cyan
            "\x1b[97m",   // Bright White
        };

        return logColorCodeStrings[static_cast<size_t>(color)];
    }

    LogColor getLogLevelColor(LogLevel level)
    {
        static const std::array<LogColor, 4> logLevelColors{
            LogColor::BrightBlack,
            LogColor::Default,
            LogColor::Yellow,
            LogColor::Red
        };

        return logLevelColors[static_cast<size_t>(level)];
    }

    const char * getLogLevelString(LogLevel level)
    {
        static const std::array<const char *, 4> logLevelStrings{
            "[Trace]",
            "[Info]",
            "[Warning]",
            "[Error]"
        };

        size_t idx = static_cast<size_t>(level);

        if (idx < logLevelStrings.size())
        {
            return logLevelStrings[idx];
        }

        return "[Undefined LogLevel]";
    }

    void log(const std::string & message, LogColor color)
    {
        static std::ofstream file("log.txt");
        file << message;

        std::cout << getColorCode(color) << message << getColorCode(LogColor::Default);
    }

    void log(LogLevel level, const std::string & message, const std::string & category, LogColor colorOverride)
    {
        std::stringstream ss;

        ss << getFormattedTimeStamp();
        ss << getLogLevelString(level);

        if (!category.empty())
            ss << "(" << category << ")";

        ss << " " << message;

        log(ss.str(), colorOverride);
    }

    void log(LogLevel level, const std::string & message, const std::string & category)
    {
        log(level, message, category, getLogLevelColor(level));
    }
}