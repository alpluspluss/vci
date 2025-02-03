#include <vci/ascii.hpp>
#include <array>

namespace vci
{
    constexpr auto TERM_RATIO = 0.5f;
    constexpr std::array<char, 10> ASCII_CHARS = {' ', '.', ':', '-', '=', '+', '*', '#', '%', '@'};

    inline char pixel_to_ascii(const uint8_t gray)
    {
        // map 0-255 to our character array indices
        // using the inverted gray value as darker pixels should map to denser characters
        const uint8_t inverted = gray;
        const size_t index = (inverted * (ASCII_CHARS.size() - 1)) / 255;
        return ASCII_CHARS[index];
    }

#if defined(__ARM_NEON)
#include <arm_neon.h>
    void rgb_to_gray(const uint8_t *rgb, uint8_t *gray, const size_t pixels)
    {
        /* standard ITU-R BT.601 weights for RGB to grayscale conversion */
        const uint8x8_t r_weights = vdup_n_u8(76);  // 0.299
        const uint8x8_t g_weights = vdup_n_u8(150); // 0.587
        const uint8x8_t b_weights = vdup_n_u8(29);  // 0.114

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
        /* standard ITU-R BT.601 weights for RGB to grayscale conversion */
        for (size_t i = 0; i < pixels; ++i)
            gray[i] = (rgb[i * 3] * 76 + rgb[i * 3 + 1] * 150 + rgb[i * 3 + 2] * 29) >> 8;
    }
#endif

    std::string frame_to_ascii(const std::vector<uint8_t> &rgb_data, const int width, const int height,
                               const int term_width)
    {
        std::vector<uint8_t> gray_data(static_cast<size_t>(width * height));
        rgb_to_gray(rgb_data.data(), gray_data.data(), width * height);
        
        const auto target_width = static_cast<std::size_t>(term_width);
        const auto width_ratio = static_cast<float>(term_width) * static_cast<float>(height);
        const auto target_height = static_cast<std::size_t>(
            (width_ratio / static_cast<float>(width)) * TERM_RATIO
        );

        const auto scale_x = static_cast<float>(width) / static_cast<float>(target_width);
        const auto scale_y = static_cast<float>(height) / static_cast<float>(target_height);

        std::string result;
        result.reserve(target_width * target_height + target_height);

        std::vector<std::size_t> x_positions(target_width);
        std::vector<std::size_t> y_positions(target_height);
        std::vector<std::size_t> row_offsets(static_cast<std::size_t>(height));
        for (std::size_t x = 0; x < target_width; ++x)
        {
            const auto scaled_x = static_cast<float>(x) * scale_x;
            x_positions[x] = std::min(
                static_cast<std::size_t>(scaled_x),
                static_cast<std::size_t>(width - 1)
            );
        }

        for (std::size_t y = 0; y < target_height; ++y)
        {
            const auto scaled_y = static_cast<float>(y) * scale_y;
            y_positions[y] = std::min(
                static_cast<std::size_t>(scaled_y),
                static_cast<std::size_t>(height - 1)
            );
        }

        for (std::size_t y = 0; y < static_cast<std::size_t>(height); ++y)
            row_offsets[y] = y * static_cast<std::size_t>(width);

        for (std::size_t y = 0; y < target_height; ++y)
        {
            const auto base_offset = row_offsets[y_positions[y]];
            for (std::size_t x = 0; x < target_width; ++x)
            {
                const auto pixel = gray_data[base_offset + x_positions[x]];
                result += pixel_to_ascii(pixel);
            }
            result += '\n';
        }

        return result;
    }
}