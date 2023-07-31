#include "stopwatch.hpp"

namespace mud
{
    void Stopwatch::start()
    {
        m_timeStart = std::chrono::high_resolution_clock::now();
    }

    double Stopwatch::stop()
    {
        m_timeStop = std::chrono::high_resolution_clock::now();
        return getElapsed();
    }

    const std::chrono::system_clock::time_point & Stopwatch::getStartTime() const
    {
        return m_timeStart;
    }

    const std::chrono::system_clock::time_point & Stopwatch::getStopTime() const
    {
        return m_timeStop;
    }

    double Stopwatch::getElapsed()
    {
        m_elapsed = m_timeStop - m_timeStart;
        return m_elapsed.count();
    }
}