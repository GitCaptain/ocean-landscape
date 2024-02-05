#ifndef UTILS_H
#define UTILS_H

#include <optional>
#include "common.h"

struct Point {
    int x;
    int y;
    Point(int x, int y): x(x), y(y) {}
    Point(): x(-1), y(-1) {}
    bool operator<(const Point& other) const {
        return x < other.x || x == other.x && y < other.y;
    }
};

int get_random_number_in_range(int l, int r);

Point create_random_point(int xmin, int xmax, int ymin, int ymax);

std::optional<Point> bfs(const Map& map, Point start);

#endif
