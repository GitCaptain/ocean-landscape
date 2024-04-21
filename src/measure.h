#ifndef MEASURE_H
#define MEASURE_H

#include <chrono>
#include "logger.h"

namespace measure {

using namespace std::chrono;

class Timer final {

    time_point<steady_clock> start;

    Timer() {
        start = steady_clock::now();
    }

    ~Timer() {
        const auto end = steady_clock::now();
        const duration<double> elapsed_seconds{end - start};
        LOG_INFO(std::cout << "Measured time: " << '\n';);
    }

};

} // namespace measure

#endif
