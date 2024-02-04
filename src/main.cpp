#include <iostream>
#include <optional>
#include <vector>
#include <string_view>
#include <charconv>
#include "generator.h"
#include "vox_writer.h"
#include "logger.h"

namespace {

    struct Args final {
        int sizex;
        int sizey;
        int years;
        std::string_view file;
    };

    const std::string_view SIZEX = "--sizex=";
    const std::string_view SIZEY = "--sizey=";
    const std::string_view YEARS = "--years=";
    const std::string_view OUTPUT = "--output=";
    const std::string_view DEFAULT_OUTPUT = "landscape.vox";
}

std::optional<Args> parse_input(std::vector<std::string_view> input) {
    Args res {0, 0, 0, DEFAULT_OUTPUT};
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
                  << OUTPUT << "file\n";
        return 0;
    }

    std::vector<std::string_view> args {argv, argv+argc};

    auto parsed_input = parse_input(args);
    if(!parsed_input.has_value()) {
        std::cerr << "Wrong input parameters\n";
        return 1;
    }
    const auto [x, y, years, file] = *parsed_input;
    LOG(std::cout << "INPUT: x = " << x
             << ", y = " << y
             << ", ye = " << years
             << ", file = " << file << '\n');

    generation::Generator g{x, y, years};
    g.generate();
    Map landscape = g.get_result();

#if 0
    VoxWriter vw;
    vw.write(landscape, file);

#else
    LOG(std::cout << "Start writing to file\n");
    vox::VoxWriter vox;
    for (int32_t x = 0; x < landscape.size(); ++x) {
        for (int32_t y = 0; y < landscape[0].size(); ++y) {
            for (int32_t z = 0; z < landscape[x][y].z; z++) {
                vox.AddVoxel(x, y, z, landscape[x][y].color);
            }
        }
    }
    vox.SaveToFile(file.data());
#endif



    return 0;
}
