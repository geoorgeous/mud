#ifndef STOPWATCH_HPP
#define STOPWATCH

#include <chrono>

namespace mud
{
    class Stopwatch
    {
    public:

        void start();

        double stop();

        const std::chrono::system_clock::time_point & getStartTime() const;

        const std::chrono::system_clock::time_point & getStopTime() const;

        double getElapsed();

    private:

        std::chrono::system_clock::time_point m_timeStart;
        std::chrono::system_clock::time_point m_timeStop;
        std::chrono::duration<double, std::milli> m_elapsed;
    };
}

#endif