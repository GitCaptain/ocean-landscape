#include <random>
#include <cstdlib>
#include <algorithm>
#include "generator.h"
#include "logger.h"

using namespace generation;

void Generator::generate() {
    LOG(std::cout << "Generation process started\n";);
    setup_map();
    split_map();
    set_height();
    set_properties();
    simulate();
}

Map Generator::get_result() const {
    return map;
}

void Generator::setup_map() {
    LOG(std::cout << "Setting up map...\n";);
    map.resize(sizex, std::vector<Voxel>(sizey, {1, -1, 0}));
    long long space = sizex * 1ll *sizey;
    int plates_count = get_random_number_in_range(5, 15);
    plates.resize(plates_count);
}

void Generator::split_map() {
    LOG(std::cout << "Split map into " << plates.size() << " plates...\n";);

    struct Point {
        int x;
        int y;
    };

    std::vector<Point> points(plates.size());
    // generate random points on plate to calcalute
    // Voronoi diagram depending on them.
    for(auto &p: points) {
        p.x = get_random_number_in_range(0, map.size());
        p.y = get_random_number_in_range(0, map[0].size());
    }

    // Discrete voronoi diagram
    for(int x = 0; x < sizex; x++) {
        for(int y = 0; y < sizey; y++) {
            int min_dist = -1;
            int point_index = 0;
            for (int i = 0; i < points.size(); i++) {
                Point &p = points[i];
                int cur_dist = std::abs(x - p.x) + std::abs(y - p.y);
                if (min_dist < 0 || cur_dist < min_dist) {
                    min_dist = cur_dist;
                    point_index = i;
                }
            }
            map[x][y].plate_ref = point_index;
        }
    }
}

void Generator::set_properties() {
    LOG(std::cout << "Set properties...\n";);
    for (Plate& p: plates) {
        p.speedX = get_random_number_in_range(2, 15);
        p.speedY = get_random_number_in_range(2, 15);
    }
    plates[0].is_edge = true;
}

void Generator::set_height() {
    LOG(std::cout << "Set heights...\n";);
    for(int i = 0; i < sizex; i++) {
        for(int j = 0; j < sizey; j++) {
            map[i][j].height = 10;
            // Palette has only 256 colors
            map[i][j].color = map[i][j].plate_ref % 256;
        }
    }
}

void Generator::simulate() {
    LOG(std::cout << "Simulation started...\n";);
    for(int delta_years = 100, current_year = 0; current_year < years;
        current_year += delta_years) {
        LOG(std::cout << "Years passed:" << current_year << '\n';);
    }
}

int Generator::get_random_number_in_range(int l, int r) const {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(l, r); // define the range
    return distr(gen);
}