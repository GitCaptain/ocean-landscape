#include "measure.h"

namespace measure {

void time_measure(
    const std::string_view result_file,
    std::function<void()> f,
    int repeats) {

    time_container tc;
    tc.reserve(repeats);
    for(int i = 0; i < repeats; i++) {
        Timer t(tc);
        f();
    }

    std::ofstream outf(result_file.data());
    for(auto &t: tc) {
        outf << t.count() << ' ';
    }

}

}
