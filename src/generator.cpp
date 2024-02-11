#include <cstdlib>
#include <algorithm>
#include <cassert>
#include <queue>
#include "generator.h"
#include "logger.h"
#include "utils.h"

using namespace generation;
using namespace utils;

void Generator::generate() {
    LOG_INFO(std::cout << "Generation process started\n";);
    setup_map();
    split_map();
    set_properties();
    set_height();
    generate_elements();
    simulate();
}

Map Generator::get_result() const {
    START();
    return map;
}

void Generator::setup_map() {
    LOG_INFO(std::cout << "Setting up map...\n";);
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
    int plates_count = get_random_number_in_range(5, 15);
    plates.resize(plates_count);
}

void Generator::split_map() {
    LOG_INFO(std::cout << "Split map into " << plates.size()
                       << " plates...\n";);

    std::vector<Point> points;
    points.reserve(plates.size());
    // generate random points on plate to calcalute
    // Voronoi diagram depending on them.
    // On the small map, there are might be collisions in random points,
    // but this is doesn't affect algorithms, so just ignore it.
    for(int i = 0; i < plates.size(); i++) {
        Point p = create_random_point(0, map.size() - 1, 0, map[0].size() - 1);
        points.emplace_back(p);
        LOG_DEBUG(std::cout << "Add point to Voronoi diagram: {"
                            << p.x << ", " << p.y << "}\n";);
    }

#ifdef DEBUG
    points = {
        {3, 1}, {2, 3}, {1, 0}, {0, 1}, {1, 4}
    };
#endif

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
    LOG_INFO(std::cout << "Set properties...\n";);
    for (Plate& p: plates) {
        p.speedX = get_random_number_in_range(2, 15);
        p.speedY = get_random_number_in_range(2, 15);
    }
    plates[0].is_edge = true;
}

void Generator::set_height() {
    LOG_INFO(std::cout << "Set heights...\n";);
    for(int i = 0; i < sizex; i++) {
        for(int j = 0; j < sizey; j++) {
            Voxel &v = map[i][j];
            v.z = 100;
            // Palette has only 256 colors
            v.color = (v.plate_ref + 1) % 256;
            LOG_DEBUG(std::cout << "Voxel Info:\nPos = {"
                                << v.x << ", " << v.y << ", " << v.z << "}\n"
                                << "Color = " << (int)v.color << '\n'
                                << "Plate = " << v.plate_ref << '\n';);
        }
    }
}

void generation::Generator::generate_elements() {
    START();
    // Generate DeapSeaBasins
#ifdef DEBUG
    int basins_cnt = 1;
#else
    int basins_cnt = get_random_number_in_range(0, 3);
#endif

# if 0
    for (int i = 0; i < basins_cnt; i++) {
        int radius = get_random_number_in_range(
                            DeepSeaBasin::min_radius,
                            DeepSeaBasin::max_radius);
        assert(radius + 1 < map.size() - radius - 1);
        int x = get_random_number_in_range(radius + 1, map.size() - radius - 1);
        assert(radius + 1 < map[0].size() - radius - 1);
        int y = get_random_number_in_range(radius + 1,
                                           map[0].size() - radius - 1);
        LOG_INFO(std::cout << "Add DeepSeaBasin { "
                           << x << ", " << y << ", " << radius << " }\n";);
        elements.emplace_back(
            std::make_unique<DeepSeaBasin>(&map[x][y], map, radius));
    }
#endif

    // Generate ContinentalMargin
#ifdef DEBUG
    int margin_cnt = 1;
#else
    int margin_cnt = get_random_number_in_range(0, 2);
#endif

    // Generate MidOceanRidge
#ifdef DEBUG
    int ridge_cnt = 1;
#else
    int ridge_cnt = get_random_number_in_range(1, 1);
#endif
    for (int i = 0; i < ridge_cnt; i++) {
        LOG_INFO(std::cout << "Add MidOceanRidge\n";);
        elements.emplace_back(
            std::make_unique<MidOceanRidge>(map));
    }
}

void Generator::simulate() {

    LOG_INFO(std::cout << "Simulation started...\n";);
    for(int delta_years = 100, current_year = 0; current_year < years;
        current_year += delta_years) {
        if (current_year % 10000 == 0) {
            LOG_INFO(std::cout << "Years passed:" << current_year << '\n';);
        }
        for (auto &landscape: elements) {
            landscape->do_iteration(delta_years);
        }
    }
}

void LandscapeElement::do_iteration(int years_delta) {
    START();
    // Add some random delay to generation
    // to get different results.
    gen_years += years_delta;
    if (delay_years > years_delta) {
        delay_years -= years_delta;
        return;
    }

    generation_step(years_delta);
}

bool LandscapeElement::point_in_map(Point p) {
    return point_in_range(p, l_map_guard, r_map_guard);
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
    START();
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
    START();

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
    START();
    int guyots_count = radius / min_radius;
    for (int i = 0; i < guyots_count; i++) {
        int g_r = get_random_number_in_range(Guyot::min_radius,
                                             Guyot::max_radius);
        int g_x = get_random_number_in_range(center->x,
                                             center->x + radius - g_r);
        int g_y = get_random_number_in_range(center->y,
                                             center->y + radius - g_r);
        LOG_DEBUG(std::cout << "Add guyot {" << g_x << ", " << g_y << ", "
                            << g_r << " }\n"
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
    START();
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
    START();
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

void MidOceanRidge::init() {
    START();
#ifdef DEBUG
    delay_years = 0;
#else
    delay_years = get_random_number_in_range(0, 5000);
#endif

#if 0
    auto opt_edge = bfs(map, Point(x, y));
    if (!opt_edge.has_value()) {
        return;
    }
#endif

    int plates_speed = get_random_number_in_range(2, 15); // cm per year

    depth_per_thousand_years = (plates_speed * 1000) / voxel_per_meter;

    find_edges();
    gen_graph();
    create_path();
}

bool MidOceanRidge::is_vertical_edge(const Vertex &v) const {
    return v.first->y == v.second->y;
}

bool MidOceanRidge::is_horisontal_edge(const Vertex &v) const {
    return v.first->x == v.second->x;
}

void MidOceanRidge::find_edges() {

    START()
    std::array<int, 4> dxs = {-1, 0, 1, 0};
    std::array<int, 4> dys = {0, -1, 0, 1};

    auto add_voxels_to_edge = [this,
                              &plates_edge=plates_edges,
                              &map_edges=map_edges]
                              (Voxel *a, Voxel *b) {
        // To avoid duplicates we only emplace voxels if a is less
        // then b, because we will get this pair two times: (a, b) and
        // (b, a)
        if (a->x < b->x || (a->x == b->x && a->y < b->y)) {
            LOG_DEBUG(std::cout << "ADD pair: { "
                                << a->x << ", " << a->y << ", " << a->plate_ref
                                << "}, { "
                                << b->x << ", " << b->y << ", " << b->plate_ref
                                << "}\n";);
            Vertex v = {a, b};
            plates_edge.emplace_back(v);
            if (is_horisontal_edge(v) && (a->x == 0 || b->x == map.size()-1) ||
                is_vertical_edge(v) && (a->y == 0 || a->y == map[0].size()-1)) {
                LOG_DEBUG(std::cout << "It is on the Map edge\n";);
                map_edges.emplace_back(v);
            }
        }
    };

    for (int x = 0; x < map.size(); x++) {
        for (int y = 0; y < map[0].size(); y++) {
            Voxel *cur = &map[x][y];
            for (int i = 0; i < dxs.size(); i++) {
                int dx = dxs[i];
                int dy = dys[i];
                Point neighbour {x + dx, y + dy};
                if (!point_in_map(neighbour)) {
                    continue;
                }
                Voxel *neighbour_v = p_voxel_from_point(map, neighbour);
                if (cur->plate_ref != neighbour_v->plate_ref) {
                    add_voxels_to_edge(cur, neighbour_v);
                }
            }
        }
    }
    LOG_DEBUG(std::cout << "Edge len: " << plates_edges.size()
                        << "\nMap edges: " << map_edges.size() << '\n';);
}

void MidOceanRidge::print_vertex(const Vertex &v) {
    LOG_DEBUG(std::cout << "{(" << v.first->x << ", " << v.first->y << "), ("
                        << v.second->x << ", " << v.second->y << ")}";);
}

void MidOceanRidge::gen_graph() {
    START()

    auto insert = [&graph = graph](const Vertex &v1, const Vertex &v2) {
        graph[v1].emplace_back(v2);
        graph[v2].emplace_back(v1);
    };

    // here we will have both (a, b) and (b, a), so
    // all conditions are single-sided to avoid duplicates in graph
    for (const auto &e1: plates_edges) {
        for (const auto &e2: plates_edges) {

            if (e1 == e2) {
                continue;
            }

            const auto [e1_v1, e1_v2] = e1;
            const auto [e2_v1, e2_v2] = e2;

            if (is_vertical_edge(e1) && is_vertical_edge(e2)) {
                if (e1_v1->x == e2_v1->x && e1_v1->y == e2_v2->y - 1) {
                    insert(e1, e2);
                }
            }

            if (is_horisontal_edge(e1) && is_horisontal_edge(e2)) {
                if (e1_v1->y == e2_v1->y && e1_v1->x == e2_v2->x - 1) {
                    insert(e1, e2);
                }
            }

            if (is_horisontal_edge(e1) && is_vertical_edge(e2) ||
                is_horisontal_edge(e2) && is_vertical_edge(e1)) {
                if (e1_v1 == e2_v1 && e1_v2->x < e2_v2->x ||
                    e1_v2 == e2_v2 && e1_v1->x < e2_v1->x ||
                    e1_v1 == e2_v2) {
                    insert(e1, e2);
                }
            }

        }
    }

#ifdef DEBUG
    LOG_DEBUG(std::cout << "Graph:\n";);
    for (const auto& v: graph) {
        print_vertex(v.first);
        LOG_DEBUG(std::cout << ": ");
         for (const auto& neigh: v.second) {
            print_vertex(neigh);
            LOG_DEBUG(std::cout << ", ");
        }
        LOG_DEBUG(std::cout << ";\n");
    }
#endif

#if 0
    // We don't need them anymore;
    plates_edges.clear();
#endif

}

void MidOceanRidge::create_path() {
    START()
    // do work
    for (const Vertex &start: map_edges) {
        const Vertex &end = bfs(start);
        try_update_path(start, end);
    }

    LOG_DEBUG(std::cout << "The longest path is: " << mor_path.size() << '\n';);

# if 0
    // We don't need them anymore;
    map_edges.clear();
    graph.clear();
    distance.clear();
#endif

    for (const auto& v: mor_path) {
        auto& [v1, v2] = v;
        v1->color = v2->color = 244;
    }

}

MidOceanRidge::Vertex MidOceanRidge::bfs(Vertex start) {
    START()
    distance.clear();
    std::queue<Vertex> q;

    q.push(start);
    Vertex far = start;
    distance[start] = 0;
    while (!q.empty()) {
        Vertex cur = q.front();
        q.pop();

        for(const Vertex& to: graph[cur]) {
            if (!distance.contains(to)) {
                distance[to] = distance[cur] + 1;
                far = to;
                q.push(to);
            }
        }
    }
    return far;
}

void MidOceanRidge::try_update_path(Vertex start, Vertex end) {
    START()
    // We have start vertex on our path which distance from itself is zero,
    // but it is still on path.
    int new_distance = distance[end] + 1;
    LOG_DEBUG(std::cout << "Check path of the size " << new_distance << '\n';);
    if (new_distance <= mor_path.size()) {
        // We have path not less then the new one
        return;
    }

    mor_path.clear();
    mor_path.resize(new_distance);
    mor_path[--new_distance] = end;
    while (new_distance) {

        for (const Vertex& from: graph[end]) {
            if (distance[from] == new_distance - 1) {
                mor_path[--new_distance] = from;
                end = from;
            }
        }

    }

}

void MidOceanRidge::generation_step(int years_delta) {
    START()

    int expected_depth = gen_years / 1000;

    if (expected_depth <= shift_already) {
        return;
    }

    int diff = expected_depth - shift_already;

    for(auto &v: mor_path) {
        int dx = 0, dy = 0;

        if (is_horisontal_edge(v)) {
            dy = 1;
        } else {
            dx = 1;
        }

        auto [vox_1, vox_2] = v;

        Point first {vox_1->x, vox_1->y};
        Point second {vox_2->x, vox_2->y};

        for(int i = 0; i < expected_depth; i++) {
            Point dfirst = first.move(-i*dx, -i*dy);
            Point dsecond = second.move(i*dx, i*dy);

            if (point_in_map(dfirst)) {
                Voxel *v = p_voxel_from_point(map, dfirst);
                v->z -= diff;
            }

            if (point_in_map(dsecond)) {
                Voxel *v = p_voxel_from_point(map, dfirst);
                v->z -= diff;
            }

        }

    }

    shift_already = expected_depth;
}
