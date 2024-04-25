#include <iostream>
#include <optional>
#include <vector>
#include <string_view>
#include <charconv>
#include "generator.h"
#include "vox_writer.h"
#include "logger.h"
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

    auto genf = [&params]() {
        generation::Generator g{params};
        g.generate();
        // Map landscape = g.get_result();
    };
    measure::do_bench(params.file.data(), genf);

// #if 0
//     VoxWriter vw;
//     vw.write(landscape, file);

// #else
//     LOG_INFO(std::cout << "Start writing to file\n";);
//     vox::VoxWriter vox;
//     for (int32_t x = 0; x < landscape.size(); ++x) {
//         for (int32_t y = 0; y < landscape[0].size(); ++y) {
//             for (int32_t z = 0; z < landscape[x][y].z; z++) {
//                 vox.AddVoxel(x, y, z, landscape[x][y].color);
//             }
//         }
//     }
//     LOG_DEBUG(std::cout << "Start saving file\n";);
//     vox.SaveToFile(params.file.data());
// #endif

    return 0;
}
