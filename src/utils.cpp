
#include <array>
#include <queue>
#include <random>
#include <set>
#include "utils.h"

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
