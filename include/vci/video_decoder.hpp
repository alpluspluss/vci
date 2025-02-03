#pragma once

#include <cstdint>
#include <vector>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

namespace vci
{
    class VideoDecoder
    {
    public:
        explicit VideoDecoder(const char *filename);

        ~VideoDecoder();

        VideoDecoder(const VideoDecoder &) = delete;

        VideoDecoder &operator=(const VideoDecoder &) = delete;

        bool read_frame(std::vector<uint8_t> &rgb_data);

        [[nodiscard]] int get_width() const noexcept;

        [[nodiscard]] int get_height() const noexcept;

        [[nodiscard]] double get_fps() const noexcept;

    private:
        AVFormatContext *fmt_ctx = nullptr;
        AVCodecContext *codec_ctx = nullptr;
        SwsContext *sws_ctx = nullptr;
        AVFrame *frame = nullptr;
        AVPacket *packet = nullptr;
        int video_stream_idx = -1;
    };
}
