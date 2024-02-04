#include <vector>
#include <bit>
#include "vox_writer.h"
#include "logger.h"


// https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt

void VoxWriter::write(Map &map, std::string_view filename) {
    LOG(std::cout << "Writing result to the " << filename << '\n';);
    outfile.open(filename.data(), std::ios::binary);
    write_content(map);
}

/*
Chunk Structure
-------------------------------------------------------------------------------
# Bytes  | Type       | Value
-------------------------------------------------------------------------------
1x4      | char       | chunk id
4        | int        | num bytes of chunk content (N)
4        | int        | num bytes of children chunks (M)

N        |            | chunk content

M        |            | children chunks
-------------------------------------------------------------------------------
*/


void VoxWriter::write_content(Map &map) {
    write_header();
    const auto [max_sizez, voxel_count] = calculate_model_size(map);
    write_main_chunk(voxel_count);
    write_size_chunk(map, max_sizez);
    write_xyzi_chunk(map, voxel_count);
}

void VoxWriter::write_header() {

/*
-------------------------------------------------------------------------------
# Bytes  | Type       | Value
-------------------------------------------------------------------------------
1x4      | char       | id 'VOX ' : 'V' 'O' 'X' 'space', 'V' is first
4        | int        | version number : 150
*/

    write_string("VOX ");
    write_uint32_t(150);
}

void VoxWriter::write_main_chunk(int voxel_count) {
    write_string("MAIN"); // id
    write_uint32_t(0); // num bytes of chunk content;
    int child_content_size = calculate_content_size(voxel_count);
    write_uint32_t(child_content_size); // num bytes of children chunks
}

void VoxWriter::write_size_chunk(Map &map, int sizez) {

/*
    Chunk id 'SIZE' : model size
    ---------------------------------------------------------------------------
    # Bytes  | Type       | Value
    ---------------------------------------------------------------------------
    4        | int        | size x
    4        | int        | size y
    4        | int        | size z : gravity direction
    ---------------------------------------------------------------------------
*/


    write_string("SIZE");
    write_uint32_t(12);
    write_uint32_t(0);
    write_uint32_t(map.size());
    write_uint32_t(map[0].size());
    write_uint32_t(sizez);
}

void VoxWriter::write_xyzi_chunk(Map &map, int voxel_count) {

/*
Chunk id 'XYZI' : model voxels, paired with the SIZE chunk
-------------------------------------------------------------------------------
# Bytes  | Type       | Value
-------------------------------------------------------------------------------
4        | int        | numVoxels (N)
4 x N    | int        | (x, y, z, colorIndex) : 1 byte for each component
-------------------------------------------------------------------------------
*/

    write_string("XYZI");
    write_uint32_t(4 + 4 * voxel_count); // num bytes of chunk content;
    write_uint32_t(0); // num bytes of children chunks
    for (int x = 0; x < map.size(); x++) {
        for (int y = 0; y < map[0].size(); y++) {
            for (int z = 0; z < map[x][y].height; z++) {
                uint32_t c = map[x][y].color;
                uint32_t pos_and_color =
                    ((x & 0xff) << 24) |
                    ((y & 0xff) << 16) |
                    ((z & 0xff) << 8)  |
                    (c & 0xff);
                write_uint32_t(pos_and_color);
            }
        }
    }
}

std::pair<int32_t, int32_t>
VoxWriter::calculate_model_size(const Map &map) const {
    int32_t max_sizez = 0;
    int32_t voxel_count = 0;
    for (const auto &vector: map) {
        for (const auto &voxel: vector) {
            max_sizez = std::max(max_sizez, voxel.height);
            voxel_count += voxel.height;
        }
    }
    LOG(std::cout << "Max landscape height is: " << max_sizez << '\n';);
    LOG(std::cout << "Overall voxels in model: " << voxel_count << '\n';);
    return {max_sizez, voxel_count};
}

int VoxWriter::calculate_content_size(int voxel_count) const {
    // Content size =
    // number of XYZI chunks * size of XYZI chunk
    // (= 4 + 4 * number of voxels + header size) +
    // (XYZI always in pair with SIZE)
    // number of SIZE chunks * size of SIZE chunk ( = 12 + header size) +
    // size of PACK chunk (= 4 + header size, optional, not used) +
    // size of RGBA chunk (= 4*256 + header size, optional, not used)
    // header size = 12
    // We currently have only one model in one XYZI chunk
    return (4 + 4 * voxel_count + 12) + (12 + 12);
}

void VoxWriter::write_string(std::string_view data) {
    outfile.write(data.data(), data.size());
}

void VoxWriter::write_uint32_t(uint32_t data) {
    // reverse byte order
    uint32_t reversed = ((data & 0xff) << 24) |
                        ((data & 0xff00) << 16) |
                        ((data & 0xff0000) << 8) |
                        ((data & 0xff000000));
    outfile.write(reinterpret_cast<const char *>(&reversed), sizeof(reversed));
}
