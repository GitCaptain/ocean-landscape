#ifndef MEASURE_H
#define MEASURE_H

#include <chrono>
#include <vector>
#include <string_view>
#include <functional>
#include <fstream>
#include "logger.h"

namespace measure {

using namespace std::chrono;
using time_container = std::vector<duration<double>>;

class Timer final {

    time_point<steady_clock> start;
    time_container& tc;

public:

    Timer(time_container& tc): tc(tc) {
        start = steady_clock::now();
    }

    ~Timer() {
        const auto end = steady_clock::now();
        const duration<double> elapsed_seconds{end - start};
        tc.push_back(elapsed_seconds);
        LOG_INFO(
            std::cout << "Measured time: " << elapsed_seconds.count() << '\n';);
    }

};

void time_measure(
    const std::string_view result_file,
    std::function<void()> f,
    int repeats = 1000);
} // namespace measure

#endif
