#include <iostream>
#include <thread>
#include <vci/ascii.hpp>
#include <vci/terminal.hpp>
#include <vci/video_decoder.hpp>

// TODO: QoL improvements for the CLI
int main(const int argc, const char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " <video_file>\n";
        return 1;
    }

    try
    {
        vci::VideoDecoder decoder(argv[1]);
        std::vector<uint8_t> rgb_frame;
        auto term_size = vci::get_terminal_size();

        const double fps = decoder.get_fps();
        const auto frame_dur = std::chrono::microseconds(static_cast<int>(1e6 / fps));

        vci::hide_cursor();
        auto next_frame_time = std::chrono::steady_clock::now();
        while (decoder.read_frame(rgb_frame))
        {
            vci::clear_screen();
            std::string ascii_frame = vci::frame_to_ascii(
                rgb_frame,
                decoder.get_width(),
                decoder.get_height(),
                term_size.cols
            );
            std::cout << ascii_frame;

            next_frame_time += frame_dur;
            std::this_thread::sleep_until(next_frame_time);
        }

        vci::show_cursor();
    }
    catch (const std::exception& e)
    {
        // I know certain OS flushes their IO buffer when the app
        // exits automatically for security reasons but this
        // is good practice
        std::cerr << "error: " << e.what() << std::endl;
        vci::show_cursor();
        return 1;
    }

    return 0;
}