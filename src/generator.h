#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include <memory>
#include "common.h"

namespace generation {

/*
Resolution: 1 voxel = 20 meters
*/

class LandscapeElement {

public:

    virtual void generation_step(int years_delta) = 0;
    virtual ~LandscapeElement() = default;
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
    DeepSeaBasin(Voxel *center, Map &map, int radius):
        center(center), map(map), radius(radius) {
            gen_years = 0;
            shift_already = 0;
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
    Map &map;
    int radius;
    int delay_years;
    int gen_years;
    int shift_already;
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
    MidOceanRidge() {}

    void generation_step(int years_delta) override;

};

class ContinentalMargin final: public LandscapeElement {

public:
    ContinentalMargin() {}

    void generation_step(int years_delta) override;

};

}

#endif
