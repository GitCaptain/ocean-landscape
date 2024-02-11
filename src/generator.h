#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include <memory>
#include <utility>
#include <map>
#include <set>
#include "common.h"
#include "utils.h"

namespace generation {

/*
Resolution: 1 voxel = 20 meters
*/

const int voxel_per_meter = 20;
using utils::Point;

class LandscapeElement {

public:
    LandscapeElement(Map &map):
        map(map), l_map_guard(0, 0), r_map_guard(map.size()-1, map[0].size()-1)
        {}
    void do_iteration(int years_delta);
    virtual ~LandscapeElement() = default;

protected:
    virtual void generation_step(int years_delta) = 0;

    bool point_in_map(Point p);

    Map &map;
    Point l_map_guard;
    Point r_map_guard;
    int gen_years = 0;
    int delay_years = 0;
    int shift_already = 0;
};

class Generator final {
public:

    Generator(int x, int y, int years): sizex(x), sizey(y), years(years) {}
    void generate();
    Map get_result() const;

private:

    void setup_map();
    void split_map();
    void set_properties();
    void generate_elements();
    void set_height();
    void simulate();

    Map map;
    std::vector<Plate> plates;
    std::vector<std::unique_ptr<LandscapeElement>> elements;
    int sizex;
    int sizey;
    int years;

};

class DeepSeaBasin final: public LandscapeElement {

public:
    DeepSeaBasin(Voxel *center, Map &map, int radius): LandscapeElement(map),
        center(center), radius(radius) {
            init();
    }

    void generation_step(int years_delta) override;

    static int min_radius;
    static int max_radius;
    // shift speed = 1m / 100 years => 1 vox per 2000 years
    static int year_per_vox_shift;
private:

    class Guyot;

    void generate_guyots();
    void init();

    Voxel * const center;
    int radius;
    std::vector<Guyot> guyots;

    class Guyot final {

    public:
        Guyot(Voxel *center, Map &map, int radius):
            center(center), map(map), radius(radius), height_multiplier(2),
            height(height_multiplier*radius) {
            init();
        }

        void generation_step();

        static int min_radius;
        static int max_radius;

    private:

        void init();

        Voxel * const center;
        Map &map;
        int radius;
        int height_multiplier;
        int height;
        int delay_years;
        int zero_level;
    };

};


class MidOceanRidge final: public LandscapeElement {
public:
    MidOceanRidge(Map &map): LandscapeElement(map) {
        init();
    }

    void generation_step(int years_delta) override;

private:

    using Vertex = std::pair<Voxel*, Voxel*>;
    void print_vertex(const Vertex &v);
    void init();
    void find_edges();
    void gen_graph();
    void create_path();

    Vertex bfs(Vertex start);
    void try_update_path(Vertex start, Vertex end);

    bool is_vertical_edge(const Vertex &v) const;
    bool is_horisontal_edge(const Vertex &v) const;

    // we gonna look for the longest path between these vertices
    std::vector<Vertex> map_edges;
    std::vector<Vertex> plates_edges;
    // TODO: create hash for Vertex and use unordered_set
    std::map<Vertex, std::vector<Vertex>> graph;
    std::map<Vertex, int> distance;
    std::vector<Vertex> mor_path;
    int depth_per_thousand_years = 0;
};

class ContinentalMargin final: public LandscapeElement {

public:
    ContinentalMargin(Map &map): LandscapeElement(map) {}

    void generation_step(int years_delta) override;

};

}

#endif
