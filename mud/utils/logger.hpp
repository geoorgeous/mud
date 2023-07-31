#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fmt/format.h>
#include <string>

namespace mud
{
    enum class LogLevel
    {
        Trace,
        Info,
        Warning,
        Error
    };

    enum class LogColor
    {
        Default,
        Black,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        BrightBlack,
        BrightRed,
        BrightGreen,
        BrightYellow,
        BrightBlue,
        BrightMagenta,
        BrightCyan,
        BrightWhite
    };

    void log(const std::string & message, LogColor color = LogColor::Default);

    void log(LogLevel level, const std::string & message, const std::string & category = std::string());

    void log(LogLevel level, const std::string & message, const std::string & category, LogColor colorOverride);


}

#endif