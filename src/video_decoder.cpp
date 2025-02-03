#include <vci/video_decoder.hpp>
#include <stdexcept>

namespace vci
{
    VideoDecoder::VideoDecoder(const char *filename)
    {
        // maybe I'll lift this off because
        // rn the warning is so damn annoying
        av_log_set_level(AV_LOG_ERROR);
        if (avformat_open_input(&fmt_ctx, filename, nullptr, nullptr) < 0)
            throw std::runtime_error("[vci]: could not open file");

        if (avformat_find_stream_info(fmt_ctx, nullptr) < 0)
            throw std::runtime_error("[vci]: could not find stream info");

        for (auto i = 0; i < fmt_ctx->nb_streams; i++)
        {
            if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                video_stream_idx = i;
                break;
            }
        }
        if (video_stream_idx == -1)
            throw std::runtime_error("No video stream found");

        const AVCodec *codec = avcodec_find_decoder(fmt_ctx->streams[video_stream_idx]->codecpar->codec_id);
        if (!codec)
        {
            throw std::runtime_error("Unsupported codec");
        }

        codec_ctx = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(codec_ctx, fmt_ctx->streams[video_stream_idx]->codecpar);

        if (avcodec_open2(codec_ctx, codec, nullptr) < 0)
            throw std::runtime_error("Could not open codec");

        frame = av_frame_alloc();
        packet = av_packet_alloc();
    }

    VideoDecoder::~VideoDecoder()
    {
        if (sws_ctx)
            sws_freeContext(sws_ctx);
        if (codec_ctx)
            avcodec_free_context(&codec_ctx);
        if (fmt_ctx)
            avformat_close_input(&fmt_ctx);
        if (frame)
            av_frame_free(&frame);
        if (packet)
            av_packet_free(&packet);
    }

    bool VideoDecoder::read_frame(std::vector<uint8_t> &rgb_data)
    {
        while (av_read_frame(fmt_ctx, packet) >= 0)
        {
            if (packet->stream_index == video_stream_idx)
            {
                int ret = avcodec_send_packet(codec_ctx, packet);
                if (ret < 0)
                {
                    av_packet_unref(packet);
                    return false;
                }

                ret = avcodec_receive_frame(codec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                {
                    av_packet_unref(packet);
                    continue;
                }

                if (!sws_ctx)
                {
                    sws_ctx = sws_getContext(
                        codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
                        codec_ctx->width, codec_ctx->height, AV_PIX_FMT_RGB24,
                        SWS_BILINEAR, nullptr, nullptr, nullptr
                    );
                }

                rgb_data.resize(codec_ctx->width * codec_ctx->height * 3);
                uint8_t *rgb_dst[] = { rgb_data.data() };
                int rgb_stride[] = { codec_ctx->width * 3 };

                sws_scale(sws_ctx, frame->data, frame->linesize, 0,
                          codec_ctx->height, rgb_dst, rgb_stride);

                av_packet_unref(packet);
                return true;
            }
            av_packet_unref(packet);
        }
        return false;
    }

    // IDK why the compiler keeps warning this is not safe
    // when I literally check for the existent of the pointer
    int VideoDecoder::get_width() const noexcept
    {
        return codec_ctx ? codec_ctx->width : 0;
    }

    // same goes for this thing
    int VideoDecoder::get_height() const noexcept
    {
        return codec_ctx ? codec_ctx->height : 0;
    }

    double VideoDecoder::get_fps() const noexcept
    {
        return fmt_ctx && video_stream_idx >= 0
            ? av_q2d(fmt_ctx->streams[video_stream_idx]->r_frame_rate)
            : 0.0;
    }
}
