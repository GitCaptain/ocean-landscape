#ifndef VOX_WRITER_H
#define VOX_WRITER_H

#include <fstream>
#include <string_view>
#include <utility>
#include "common.h"

class VoxWriter final {

/*
Currently model might be 256*256*256 maximum due to magical voxel restrictions
*/

public:
    void write(Map &map, std::string_view filename);

private:
    void write_header();
    void write_content(Map &map);
    void write_main_chunk(int voxel_count);
    void write_size_chunk(Map &map, int sizez);
    void write_xyzi_chunk(Map &map, int voxel_count);
    void write_uint32_t(uint32_t data);
    void write_string(std::string_view data);
    std::pair<int32_t, int32_t> calculate_model_size(const Map &map) const;
    int calculate_content_size(int voxel_count) const;
    std::ofstream outfile;
};

#endif
