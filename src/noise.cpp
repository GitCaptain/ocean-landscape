#include <vector>
#include <utility>
#include <cassert>
#include <cstdlib>
#include "noise.h"
#include "logger.h"

namespace {

using namespace std;

long double lerp(long double t, long double a, long double b) {
    return a + t * (b - a);
}

class Perlin final {

    using vec2 = pair<long double, long double>;

    vec2 get_gradient(int x, int y) {
        switch (abs(x+y+x*y)%4) {
        case 0:
            return {0, 1};
        case 1:
            return {0, -1};
        case 2:
            return {-1, 0};
        case 3:
            return {1, 0};
        }
        assert(false && "End of non-void function without return");
    }

    long double fade(long double x) {
        return x*x*x*(6*x*x - 15*x + 10);
    }

    long double dot(vec2 a, vec2 b) {
        return a.first * b.first + a.second * b.second;
    }

public:

    long double get_noise(int x, int y) {

        // for easy displacement
        // assert(x >= 0 && y >= 0 && "expected non negative coords");

        // create synthetic displacement. Note: it should always be same for
        // same x and y;
        // float dx = (x + y) * 2 / (max(x, y) / 3 + 1);
        // float dy = (x * y) * 2 / (max(x, y) / 3 + 1);

        #define dotprod(xx, yy) dot({(xx), (yy)}, get_gradient((xx), (yy)))

        const auto n00 = dotprod(x-1, y);
        const auto n01 = dotprod(x, y-1);
        const auto n02 = dotprod(x+1, y);
        const auto n03 = dotprod(x, y+1);

        #undef dotprod
        const auto v = fade(x);
        const auto u = fade(y);

        return lerp(u, lerp(v, n00, n01), lerp(v, n02, n03));
    }

};

}


namespace Noise {

void make_noise(Map& map, int noise_min, int noise_max) {
    LOG_DEBUG(std::cout << "Start making noise\n";);
    Perlin p;
    for(int i = 0; i < map.size(); i++) {
        for (int j = 0; j < map[0].size(); j++) {
            map[i][j].z = static_cast<int>(
                lerp(p.get_noise(i, j), noise_min, noise_max));
        }
    }
}

}
