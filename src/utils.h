#ifndef UTILS_H
#define UTILS_H

#include <optional>
#include "common.h"
namespace utils {

struct Point {
    int x;
    int y;
    Point(int x, int y): x(x), y(y) {}
    Point(): x(-1), y(-1) {}
    bool operator<(const Point& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }
    void set(int x, int y) {
        this->x = x;
        this->y = y;
    }

    Point move(int dx, int dy) {
        return {x + dx, y + dy};
    }

};

bool point_in_range(Point p, Point l_guard, Point r_guard);

Voxel voxel_from_point(const Map& map, Point p);

Voxel* p_voxel_from_point(Map& map, Point p);

int get_random_number_in_range(int l, int r);

Point create_random_point(int xmin, int xmax, int ymin, int ymax);

#if 0
std::optional<Point> bfs(const Map& map, Point start);
#endif
}

#endif
