#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace vci
{
    std::string frame_to_ascii(const std::vector<uint8_t>& rgb_data, int width, int height,
                          int term_width, int term_height);

    /* SIMD */
    void rgb_to_gray(const uint8_t* rgb, uint8_t* gray, size_t pixels);
}