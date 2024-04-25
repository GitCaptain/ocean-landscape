#include "measure.h"
#include <algorithm>
#include <numeric>

namespace measure {

time_container time_measure(
    std::function<void()> f,
    int repeats) {

    time_container tc;
    tc.reserve(repeats);
    for(int i = 0; i < repeats; i++) {
        Timer t(tc);
        f();
    }
    return tc;

}

void print_stats(const std::string_view result_file, time_container& tc) {
    std::ofstream outf(result_file.data());
    for(auto &t: tc) {
        outf << t.count() << ' ';
    }
    // outf << "\nstaticstics:\nmin max avg median";
    // std::sort(tc.begin(), tc.end());
    // double median = tc[tc.size()/2].count();
    // double avg = std::accumulate(tc.begin(), tc.end(), 0.0) / tc.size();
    // double min = tc[0].count();
    // double max = tc.back().count();
    // outf << min << ' ' << max << ' ' << avg << ' ' << median;
}

void do_bench(
    const std::string_view result_file,
    std::function<void()> f,
    int repeats) {

    auto measures = time_measure(f, repeats);
    print_stats(result_file, measures);

}

}
