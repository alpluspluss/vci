#include <vci/ascii.hpp>

namespace vci
{
    constexpr char ASCII_CHARS[] = " .:-=+*#%@";
    constexpr int ASCII_CHARS_LEN = sizeof(ASCII_CHARS) - 1;

    char pixel_to_ascii(const uint8_t gray)
    {
        const int index = (gray * (ASCII_CHARS_LEN - 1)) / 255;
        return ASCII_CHARS[index];
    }

#if defined(__ARM_NEON)
#include <arm_neon.h>
    void rgb_to_gray(const uint8_t *rgb, uint8_t *gray, size_t pixels)
    {
        const uint8x8_t r_weights = vdup_n_u8(76);
        const uint8x8_t g_weights = vdup_n_u8(150);
        const uint8x8_t b_weights = vdup_n_u8(29);

        for (size_t i = 0; i < pixels; i += 8)
        {
            uint8x8x3_t rgb_pixels = vld3_u8(rgb + i * 3);

            uint16x8_t r = vmull_u8(rgb_pixels.val[0], r_weights);
            uint16x8_t g = vmull_u8(rgb_pixels.val[1], g_weights);
            uint16x8_t b = vmull_u8(rgb_pixels.val[2], b_weights);

            uint16x8_t sum = vaddq_u16(vaddq_u16(r, g), b);
            uint8x8_t result = vshrn_n_u16(sum, 8);

            vst1_u8(gray + i, result);
        }
    }
#else
    void rgb_to_gray(const uint8_t *rgb, uint8_t *gray, size_t pixels)
    {
        for (size_t i = 0; i < pixels; ++i)
        {
            gray[i] = (rgb[i * 3] * 76 + rgb[i * 3 + 1] * 150 + rgb[i * 3 + 2] * 29) >> 8;
        }
    }
#endif

    std::string frame_to_ascii(const std::vector<uint8_t>& rgb_data, const int width, const int height,
                          const int term_width, const int term_height)
    {
        std::vector<uint8_t> gray_data(static_cast<size_t>(width * height));
        rgb_to_gray(rgb_data.data(), gray_data.data(), width * height);

        const float scale_x = static_cast<float>(width) / static_cast<float>(term_width);
        const float scale_y = static_cast<float>(height) / (static_cast<float>(term_height) * 2.0f);

        std::string result;
        result.reserve(term_width * term_height + term_height); // +term_height for newlines

        /* pre-calculate the positions and offsets
          for better cache coherency
          TODO: loop unrolling */
        std::vector<int> x_positions(static_cast<size_t>(term_width));
        for (auto x = 0; x < term_width; ++x)
        {
            const auto scaled_x = static_cast<int>(static_cast<float>(x) * scale_x);
            x_positions[static_cast<size_t>(x)] = std::min(scaled_x, width - 1);
        }

        std::vector<int> y_positions(static_cast<size_t>(term_height));
        for (auto y = 0; y < term_height; ++y)
        {
            const auto scaled_y = static_cast<int>(static_cast<float>(y) * scale_y);
            y_positions[static_cast<size_t>(y)] = std::min(scaled_y, height - 1);
        }

        std::vector<int> row_offsets(static_cast<size_t>(height));
        for (auto y = 0; y < height; ++y)
        {
            row_offsets[static_cast<size_t>(y)] = y * width;
        }

        for (auto y = 0; y < term_height; ++y)
        {
            const int base_offset = row_offsets[static_cast<size_t>(y_positions[static_cast<size_t>(y)])];

            for (auto x = 0; x < term_width; ++x)
            {
                const uint8_t pixel = gray_data[(
                    base_offset + x_positions[static_cast<size_t>(x)]
                )];
                result += pixel_to_ascii(pixel);
            }
            result += '\n';
        }

        return result;
    }
}