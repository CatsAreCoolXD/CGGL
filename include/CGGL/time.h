#ifndef CGGL_TIME_H
#define CGGL_TIME_H

#include <chrono>

namespace ch = std::chrono;

namespace cg {
    using TimePoint = ch::time_point<ch::high_resolution_clock>;
    using Years = ch::years;
    using Months = ch::months;
    using Weeks = ch::weeks;
    using Days = ch::days;
    using Hours = ch::hours;
    using Minutes = ch::minutes;
    using Seconds = ch::duration<double>; // Fractional seconds for maximum precision
    using MilliSeconds = ch::milliseconds;
    using MicroSeconds = ch::microseconds;
    using NanoSeconds = ch::nanoseconds;

    class Clock {
        public:
            Clock() {}

            TimePoint Start();

            // Returns time elapsed in int64_t, and if the duration type is seconds it returns a double.
            template<typename Duration>
            double TimeElapsed() {
                return ch::duration_cast<Duration>((started ? cg::Clock::Now() : clockEnd) - clockStart).count();
            }

            TimePoint Stop();

            TimePoint Now();

            bool IsTicking() const { return started; }
            TimePoint GetStart() const { return clockStart; }
        private:
            TimePoint clockStart, clockEnd;
            bool started = false;
    };
}

#endif