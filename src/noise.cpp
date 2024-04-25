#include <vector>
#include <utility>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <random>
#include "noise/FastNoiseLite.h"
#include "noise.h"
#include "logger.h"

/*
namespace {

using namespace std;

class Perlin final {
private:
    vector<int> perm;
    vector<pair<double, double>> gradients;

    double fade(double t) const {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    double grad(int hash, double x, double y) const {
        auto& gradient = gradients[hash & 7];
        return x * gradient.first + y * gradient.second;
    }

    int hash(int x, int y) const {
        return perm[(perm[x & 255] + y) & 255];
    }

    double warp(double x) {
        if ( x < -1.0) return -1.0;
        if ( x > 1.0) return 1.0;
        return x;
    }

public:
    Perlin() {
        // Инициализация перестановки
        perm.resize(256);
        iota(perm.begin(), perm.end(), 0);
        random_device rd;
        mt19937 g(rd());
        shuffle(perm.begin(), perm.end(), g);
        perm.insert(perm.end(), perm.begin(), perm.end());

        // Градиентные векторы
        gradients = {
            {1.0, 0.0}, {-1.0, 0.0}, {0.0, 1.0}, {0.0, -1.0},
            {0.707, 0.707}, {-0.707, 0.707}, {0.707, -0.707}, {-0.707, -0.707}
        };
    }

    double get_noise(double x, double y) {
        LOG_INFO(cout << "noise for (" << x << ' ' << y << ")\n";);
        int xi = (int)floor(x) & 255;
        int yi = (int)floor(y) & 255;
        double xf = x - floor(x);
        double yf = y - floor(y);

        int g00 = hash(xi, yi);
        int g01 = hash(xi, yi + 1);
        int g10 = hash(xi + 1, yi);
        int g11 = hash(xi + 1, yi + 1);

        double n00 = grad(g00, xf, yf);
        double n01 = grad(g01, xf, yf - 1);
        double n10 = grad(g10, xf - 1, yf);
        double n11 = grad(g11, xf - 1, yf - 1);

        double u = fade(xf);
        double v = fade(yf);

        double nx0 = lerp(n00, n10, u);
        double nx1 = lerp(n01, n11, u);

        double res = (warp(lerp(nx0, nx1, v)) + 1) / 2;

        return res;
    }
};

int root_int(int x) {
    int i = 0;
    for(; i * i < x; i++);
    return i;
}

}

*/

namespace {
    float warp(float x) {
        if ( x < -1.0) return -1.0;
        if ( x > 1.0) return 1.0;
        return x;
    }
}

namespace Noise {


/*
void make_noise(Map& map, int noise_min, int noise_max) {
    LOG_DEBUG(cout << "Start making noise\n";);
    LOG_DEBUG(cout << noise_min << ' ' << noise_max << '\n';);
    Perlin p;
    int w = map.size();
    int h = map[0].size();
    int cellsize_w = w/2; root_int(w);
    int cellsize_h = h/2; //root_int(h);
    for(int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            double x = static_cast<double>(i%cellsize_w)/cellsize_w;
            double y = static_cast<double>(j%cellsize_h)/cellsize_h;
            double noise = p.get_noise(x, y);
            map[i][j].z = static_cast<int>(
                lerp(noise_min, noise_max, noise));
            LOG_INFO(cout << " (" << map[i][j].z << ") ";);
        }
        LOG_INFO(cout << '\n';);
    }
}
*/

void make_noise(Map& map, int noise_min, int noise_max) {
    LOG_DEBUG(std::cout << "Start making noise\n";);
    LOG_DEBUG(std::cout << noise_min << ' ' << noise_max << '\n';);
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    int w = map.size();
    int h = map[0].size();
    for(int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            float noise_res = warp(noise.GetNoise((float)i, (float)j)); // -1..1
            noise_res += 1; noise_res /= 2; // 0..1
            map[i][j].z = std::lerp(noise_min, noise_max, noise_res);
            LOG_DEBUG(std::cout << noise_res << " (" << map[i][j].z << ") ";);
        }
        LOG_DEBUG(std::cout << '\n';);
    }
}


}
