#include "CGGL/time.h"

namespace cg {
    TimePoint Clock::Start(){
        clockStart = ch::high_resolution_clock::now();
        started = true;
        return clockStart;
    }
}

cg::TimePoint cg::Clock::Stop(){
    started = false;
    clockEnd = cg::Clock::Now();
    return clockEnd;
}

cg::TimePoint cg::Clock::Now()
{
    return ch::high_resolution_clock::now();
}