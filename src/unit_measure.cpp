#include <iostream>
#include <optional>
#include <vector>
#include <string_view>
#include <charconv>
#include <cassert>
#include <string>
#include "logger.h"
#include "generator.h"
#include "measure.h"

namespace {

    const std::string_view SIZEX = "--sizex=";
    const std::string_view SIZEY = "--sizey=";
    const std::string_view YEARS = "--years=";
    const std::string_view OUTPUT = "--output=";
    const std::string_view DEFAULT_OUTPUT = "landscape.vox";

    const std::string_view MID_OCEAN_RIDGE_CNT = "--mor-cnt=";
    const std::string_view DEEP_SEA_BASIN_CNT = "--basin-cnt=";
    const std::string_view CONTINENTAL_MARGIN_CNT = "--margin-cnt=";

}

std::optional<GenParams> parse_input(std::vector<std::string_view> input) {
    GenParams res {0, 0, 0, -1, -1, -1, DEFAULT_OUTPUT};
    bool x = false, y = false, years = false;

    auto str2int = [](std::string_view full, std::string_view extra, int &out) {
        auto int_part = full.substr(extra.size());
        auto res = std::from_chars(int_part.begin(), int_part.end(), out);
        return res.ec != std::errc::invalid_argument &&
               res.ec != std::errc::result_out_of_range;
    };

    for (const auto param: input) {
        if(param.starts_with(SIZEX)) {
            if(!str2int(param, SIZEX, res.sizex)) return {};
            x = true;
        }
        if(param.starts_with(SIZEY)) {
            if(!str2int(param, SIZEY, res.sizey)) return {};
            y = true;
        }
        if(param.starts_with(YEARS)) {
            if(!str2int(param, YEARS, res.years)) return {};
            years = true;
        }
        if(param.starts_with(MID_OCEAN_RIDGE_CNT)) {
            if(!str2int(param, MID_OCEAN_RIDGE_CNT, res.mor_cnt)) return {};
            years = true;
        }
        if(param.starts_with(DEEP_SEA_BASIN_CNT)) {
            if(!str2int(param, DEEP_SEA_BASIN_CNT, res.basin_cnt)) return {};
            years = true;
        }
        if(param.starts_with(CONTINENTAL_MARGIN_CNT)) {
            if(!str2int(param, CONTINENTAL_MARGIN_CNT, res.margin_cnt)) return {};
            years = true;
        }
        if(param.starts_with(OUTPUT)) {
            res.file = param.substr(OUTPUT.size());
        }
    }

    if (!x || !y || !years) {
        return {};
    }

    return res;
}

using namespace generation;
using namespace utils;

void measure_elements(const GenParams& params) {
    START();
    // Generate DeapSeaBasins


    int sizex = params.sizex;
    int sizey = params.sizey;

    const char *file_suffix = params.file.data();

#define measureUnit(Unit, ...) do { \
    Generator g{params}; \
    g.setup_map(); \
    g.split_map(); \
    g.set_properties(); \
    g.set_height(); \
    auto map = g.get_result(); \
    auto f = [&]() { std::make_unique<Unit>(__VA_ARGS__); }; \
    measure::do_bench(#Unit "Init", f); \
    auto unit = std::make_unique<Unit>(__VA_ARGS__); \
    auto ff = [&]() {unit->do_iteration(100);}; \
    measure::do_bench((#Unit "Iteration") + std::string(file_suffix), ff); \
} while(0);

{
    int radius = get_random_number_in_range(
                        DeepSeaBasin::min_radius,
                        DeepSeaBasin::max_radius);
    assert(radius + 1 < sizex - radius - 1);
    int x = get_random_number_in_range(radius + 1, sizex - radius - 1);
    assert(radius + 1 < sizey - radius - 1);
    int y = get_random_number_in_range(radius + 1,
                                        sizey - radius - 1);
    LOG_INFO(std::cout << "Add DeepSeaBasin { "
                        << x << ", " << y << ", " << radius << " }\n";);
    measureUnit(DeepSeaBasin, &map[x][y], map, radius);
}
{
    int x = get_random_number_in_range(0, sizex - 1);
    int y = get_random_number_in_range(0, sizey - 1);
    LOG_INFO(std::cout << "Add Continental Margin {"
                        << x << ", " << y << "}\n";);
    measureUnit(ContinentalMargin, map, x, y);
}
{
    LOG_INFO(std::cout << "Add MidOceanRidge\n";);
    measureUnit(MidOceanRidge, map);
}
#undef measureUnit
}

void measure_generator(const GenParams& params) {

    const char *file_suffix = params.file.data();
    std::vector<std::function<void(Generator&)>> call_before;

#define measureMethod(method) do {\
    Generator g{params}; \
    for (auto e: call_before) {e(g);} \
    auto f = [&]() { g.method(); }; \
    measure::do_bench(#method + std::string(file_suffix), f); \
    call_before.push_back( \
        std::bind(&Generator::method, std::placeholders::_1)); \
} while(0);

    measureMethod(setup_map);
    measureMethod(split_map);
    measureMethod(set_properties);
    measureMethod(set_height);

#undef measureMethod
}

int main(int argc, char **argv) {

    if (argc < 4) {
        std::cout << "Usage: " << argv[0] << ' '
                  << SIZEX << "X "
                  << SIZEY << "Y "
                  << YEARS << "N "
                  << "[ " << OUTPUT << "file ] "
                  << "[ " << MID_OCEAN_RIDGE_CNT << "cnt ] "
                  << "[ " << DEEP_SEA_BASIN_CNT << "cnt ] "
                  << "[ " << CONTINENTAL_MARGIN_CNT << "cnt ]\n";

        return 0;
    }

    std::vector<std::string_view> args {argv, argv+argc};

    auto parsed_input = parse_input(args);
    if(!parsed_input.has_value()) {
        std::cerr << "Wrong input parameters\n";
        return 1;
    }
    const auto params = *parsed_input;
    LOG_DEBUG(std::cout << "INPUT: x = " << params.x
                        << ", y = " << params.y
                        << ", ye = " << params.years
                        << ", mor = " << params.mor_cnt
                        << ", basin = " << params.basin_cnt
                        << ", margin = " << params.margin_cnt
                        << ", file = " << params.file << '\n');


    measure_generator(params);
    measure_elements(params);

    return 0;
}
