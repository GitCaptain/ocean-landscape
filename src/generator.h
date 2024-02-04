#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include "common.h"

namespace generation {

class Generator final {
public:

    Generator(int x, int y, int years): sizex(x), sizey(y), years(years) {}
    void generate();
    Map get_result() const;

private:

    void setup_map();
    void split_map();
    void set_properties();
    void set_height();
    void simulate();

    int get_random_number_in_range(int l, int r) const;

    Map map;
    std::vector<Plate> plates;
    int sizex;
    int sizey;
    int years;

};

}

#endif
