
#include <array>
#include <queue>
#include <random>
#include <vector>
#include <set>
#include "utils.h"

namespace utils {

bool point_in_range(Point p, Point l_guard, Point r_guard) {
    return l_guard.x <= p.x && p.x <= r_guard.x &&
           l_guard.y <= p.y && p.y <= r_guard.y;
}

Voxel voxel_from_point(const Map& map, Point p) {
    return map[p.x][p.y];
}

Voxel* p_voxel_from_point(Map& map, Point p) {
    return &map[p.x][p.y];
}

int get_random_number_in_range(int l, int r) {
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(l, r); // define the range
    return distr(gen);
}

Point create_random_point(int xmin, int xmax, int ymin, int ymax) {
    return { get_random_number_in_range(xmin, xmax) ,
             get_random_number_in_range(ymin, ymax) };
}

# if 0
std::optional<Point> bfs(const Map& map, Point start) {
    std::queue<Point> q;
    std::set<Point> used;
    std::array<int, 3> dxs = {-1, 0, 1};
    std::array<int, 3> dys = {-1, 0, 1};

    int minx = 0, maxx = map.size();
    int miny = 0, maxy = map[0].size();

    q.push(start);
    while (!q.empty()) {
        Point p = q.front();
        q.pop();
        for (auto dx: dxs) {
            for (auto dy: dys) {
                if (!(p.x + dx >= minx && p.x + dx < maxx &&
                    p.y + dy >= miny && p.y + dy < maxy)) {
                    continue;
                }
                Point to {p.x + dx, p.y + dy};
                if (used.contains(to)) {
                    continue;
                }
                if (map[to.x][to.y].plate_ref !=
                    map[start.x][start.y].plate_ref) {
                    return p; // on current plate, but neighbour to other plate
                }
                used.insert(to);
            }
        }
    }
    // Probably this is single plate landscape
    return {};
}

std::vector<Point> bfs2(const Map& map, Point start) {
    std::queue<Point> q;
    std::set<Point> used;
    std::set<int> used_plates;

    std::array<int, 3> dxs = {-1, 0, 1};
    std::array<int, 3> dys = {-1, 0, 1};

    int minx = 0, maxx = map.size();
    int miny = 0, maxy = map[0].size();
    Point l_guard {minx, miny};
    Point r_guard {maxx, maxy};

    Voxel start_v = voxel_from_point(map, start);
    used_plates.insert(start_v.plate_ref);
    int neighbour_plate_ref = -1;
    used.insert(start);
    q.push(start);
    while (!q.empty()) {
        Point p = q.front();
        q.pop();
        for (auto dx: dxs) {
            for (auto dy: dys) {
                Point to {p.x + dx, p.y + dy};
                if (!point_in_range(to, l_guard, r_guard)) {
                    continue;
                }
                if (used.contains(to)) {
                    continue;
                }
                Voxel to_v = voxel_from_point(map, to);
                used.insert(to);
                if (used_plates.contains(to_v.plate_ref)) {
                    continue;
                }
                if (neighbour_plate_ref < 0) {
                    neighbour_plate_ref = to_v.plate_ref;
                }
                if (to_v.plate_ref != neighbour_plate_ref) {
                    z
                }
            }
        }
    }
    // Probably this is single plate landscape
    return {};
}
# endif

}
