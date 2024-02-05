#include <cstdlib>
#include <algorithm>
#include <cassert>
#include "generator.h"
#include "logger.h"
#include "utils.h"

using namespace generation;

void Generator::generate() {
    LOG(std::cout << "Generation process started\n";);
    setup_map();
    split_map();
    set_properties();
    set_height();
    generate_elements();
    simulate();
}

Map Generator::get_result() const {
    return map;
}

void Generator::setup_map() {
    LOG(std::cout << "Setting up map...\n";);
    map.resize(sizex, std::vector<Voxel>(sizey));
    for (int x = 0; x < sizex; x++) {
        for (int y = 0; y < sizey; y++) {
            Voxel &vox = map[x][y];
            vox.color = 0;
            vox.plate_ref = -1;
            vox.x = x;
            vox.y = y;
            vox.z = 0;
        }
    }
    long long space = sizex * 1ll *sizey;
    int plates_count = get_random_number_in_range(5, 15);
    plates.resize(plates_count);
}

void Generator::split_map() {
    LOG(std::cout << "Split map into " << plates.size() << " plates...\n";);

    std::vector<Point> points;
    points.reserve(plates.size());
    // generate random points on plate to calcalute
    // Voronoi diagram depending on them.
    for(int i = 0; i < plates.size(); i++) {
        points.emplace_back(
                create_random_point(0, map.size(), 0, map[0].size()));
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

void generation::Generator::generate_elements() {

    // Generate DeapSeaBasins
#ifdef DEBUG
    int basins_cnt = 1;
#else
    int basins_cnt = get_random_number_in_range(0, 3);
#endif

    for (int i = 0; i < basins_cnt; i++) {
        int radius = get_random_number_in_range(
                            DeepSeaBasin::min_radius,
                            DeepSeaBasin::max_radius);
        assert(radius + 1 < map.size() - radius - 1);
        int x = get_random_number_in_range(radius + 1, map.size() - radius - 1);
        assert(radius + 1 < map[0].size() - radius - 1);
        int y = get_random_number_in_range(radius + 1,
                                           map[0].size() - radius - 1);
        LOG(std::cout << "Add DeepSeaBasin { "
                      << x << ", " << y << ", " << radius << " }\n";);
        elements.emplace_back(
            std::make_unique<DeepSeaBasin>(&map[x][y], map, radius));
    }

    // Generate MidOceanRidge
#ifdef DEBUG
    int ridge_cnt = 1;
#else
    int ridge_cnt = get_random_number_in_range(0, 2);
#endif
    for (int i = 0; i < ridge_cnt; i++) {
        int x = get_random_number_in_range(0, map.size());
        int y = get_random_number_in_range(0, map[0].size());
        LOG(std::cout << "Add MidOceanRidge { " << x << ", " << y << " }\n";);
        elements.emplace_back(
            std::make_unique<MidOceanRidge>(map, x, y));
    }

    // Generate ContinentalMargin
#ifdef DEBUG
    int margin_cnt = 1;
#else
    int margin_cnt = get_random_number_in_range(0, 1);
#endif

}

void Generator::set_height() {
    LOG(std::cout << "Set heights...\n";);
    for(int i = 0; i < sizex; i++) {
        for(int j = 0; j < sizey; j++) {
            map[i][j].z = 100;
            // Palette has only 256 colors
            map[i][j].color = (map[i][j].plate_ref + 1) % 256;
        }
        LOG(std::cout << "");
    }
}

void Generator::simulate() {

    LOG(std::cout << "Simulation started...\n";);
    for(int delta_years = 100, current_year = 0; current_year < years;
        current_year += delta_years) {
        if (current_year % 1000 == 0) {
            LOG(std::cout << "Years passed:" << current_year << '\n';);
        }
        for (auto &landscape: elements) {
            landscape->generation_step(delta_years);
        }
    }
}

#define COMMON_CALC(map, center, radius) \
    const int x0 = center->x; \
    const int y0 = center->y; \
    \
    const int xmin = std::max(0, x0 - radius); \
    const int xmax = std::min(map.size(), \
                              static_cast<size_t>(x0 + radius + 1)); \
    \
    const int ymin = std::max(0, y0 - radius); \
    const int ymax = std::min(map[0].size(), \
                              static_cast<size_t>(y0 + radius + 1)); \
    const long long  rsq = radius*1ll*radius;

void DeepSeaBasin::init() {

    COMMON_CALC(map, center, radius);

#ifdef DEBUG
    delay_years = 0;
#else
    delay_years = get_random_number_in_range(0, 5000);
#endif

    for (int x = xmin; x < xmax; x++) {
        for (int y = ymin; y < ymax; y++) {
            int dx = x - x0;
            int dy = y - y0;
            Voxel &vox = map[x][y];
            if (dx *1ll*dx + dy*1ll*dy <= rsq) {
                vox.color = 255 - vox.color; // change color to see difference
            }
        }
    }
    generate_guyots();
}

void DeepSeaBasin::generation_step(int years_delta) {

    // Add some random delay to generation
    // to get different results.
    if (delay_years > years_delta) {
        delay_years -= years_delta;
        return;
    }

    gen_years += years_delta;
    int current_shift = 0;
    if (gen_years / year_per_vox_shift > shift_already) {
        current_shift = gen_years / year_per_vox_shift - shift_already;
        assert(current_shift >= 0);
    }

    if (!current_shift) return;

    COMMON_CALC(map, center, radius);

    for (int x = xmin; x < xmax; x++) {
        for (int y = ymin; y < ymax; y++) {
            int dx = x - x0;
            int dy = y - y0;
            Voxel &vox = map[x][y];
            if (vox.z >= current_shift && dx * 1ll * dx + dy * 1ll *dy <= rsq) {
                vox.z -= current_shift;
            }
        }
    }
    shift_already += current_shift;

    for (auto &g: guyots) {
        g.generation_step();
    }
}

void DeepSeaBasin::generate_guyots() {
    int guyots_count = radius / min_radius;
    for (int i = 0; i < guyots_count; i++) {
        int g_r = get_random_number_in_range(Guyot::min_radius,
                                             Guyot::max_radius);
        int g_x = get_random_number_in_range(center->x,
                                             center->x + radius - g_r);
        int g_y = get_random_number_in_range(center->y,
                                             center->y + radius - g_r);
        LOG(std::cout << "Add guyot {" << g_x << ", " << g_y << ", " << g_r
                      << " }\n"
                      << "to basin { " << center->x << ", " << center->y
                      << " }\n";);
        assert(g_x > 0 && g_x < map.size());
        assert(g_y > 0 && g_y < map[0].size());
        guyots.emplace_back(&map[g_x][g_y], map, g_r);
    }
}

int DeepSeaBasin::min_radius = 50;
int DeepSeaBasin::max_radius = 100;
int DeepSeaBasin::year_per_vox_shift = 2000;

void DeepSeaBasin::Guyot::init() {

#ifdef DEBUG
    delay_years = 0;
#else
    delay_years = get_random_number_in_range(0, 1000);
#endif

    COMMON_CALC(map, center, radius);
    zero_level = map[xmin][ymin].z;
    for (int x = xmin; x < xmax; x++) {
        for (int y = ymin; y < ymax; y++) {
            int dx = std::abs(x - x0);
            int dy = std::abs(y - y0);
            Voxel &vox = map[x][y];
            if (dx *1ll*dx + dy*1ll*dy <= rsq) {
                vox.color = 255 - vox.color; // change color to see difference
                vox.z = zero_level + height - std::max(dx, dy) * height_multiplier;
            }
        }
    }

}

void DeepSeaBasin::Guyot::generation_step() {

    // We get here each time DeepSeaBasin make shift.
    zero_level--;

    // Add some random delay to generation
    // to get different results.
    if (delay_years > DeepSeaBasin::year_per_vox_shift) {
        delay_years -= DeepSeaBasin::year_per_vox_shift;
        return;
    }

    // Remove one level on every step

    COMMON_CALC(map, center, radius);

    for (int x = xmin; x < xmax; x++) {
        for (int y = ymin; y < ymax; y++) {
            int dx = x - x0;
            int dy = y - y0;
            Voxel &vox = map[x][y];
            if (vox.z == zero_level + height && dx *1ll*dx + dy*1ll*dy <= rsq) {
                vox.z--;
            }
        }
    }
    height--;
}

int DeepSeaBasin::Guyot::min_radius = 20;
int DeepSeaBasin::Guyot::max_radius = 30;

#undef COMMON_CALC

void MidOceanRidge::init(int x, int y) {

#ifdef DEBUG
    delay_years = 0;
#else
    delay_years = get_random_number_in_range(0, 5000);
#endif

    auto opt_edge = bfs(map, Point(x, y));
    if (!opt_edge.has_value()) {
        return;
    }
    // fill int path from this point
}

void MidOceanRidge::generation_step(int years_delta) {

    if (delay_years > years_delta) {
        delay_years -= years_delta;
        return;
    }



}
