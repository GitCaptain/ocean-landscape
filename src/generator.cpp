#include <random>
#include "generator.h"
#include "logger.h"

using namespace generation;

void Generator::generate() {
    LOG(std::cout << "Generation process started\n";);
    setup_map();
    split_map();
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
    int plates_count = 5; // get_random_number_in_range(space / 200, space / 100);
    plates.resize(plates_count);
}

void Generator::split_map() {
    // Replace this with something like Voronoi diagram
    LOG(std::cout << "Split map...\n";);
    for(int i = 0; i < sizex; i++) {
        for(int j = 0; j < sizey; j++) {
            if (i > sizex / 4 &&
                i < sizex / 4 * 3 &&
                j > sizey / 4 &&
                j < sizey / 4 * 3) {
                    map[i][j].plate_ref = 4;
                }
            else if (i < sizex / 2 && j < sizex / 2) {
                map[i][j].plate_ref = 0;
            }
            else if (i >= sizex / 2 && j < sizex / 2) {
                map[i][j].plate_ref = 1;
            }
            else if (i >= sizex / 2 && j >= sizex / 2) {
                map[i][j].plate_ref = 2;
            }
            else if (i < sizex / 2 && j >= sizex / 2) {
                map[i][j].plate_ref = 3;
            }
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
            map[i][j].height = 1;
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