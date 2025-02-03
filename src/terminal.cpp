#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>
#include <vci/terminal.hpp>

namespace vci
{
    TerminalSize get_terminal_size()
    {
        winsize w{};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return { w.ws_row, w.ws_col };
    }

    void clear_screen()
    {
        std::cout << "\033[2J\033[H";
    }

    void hide_cursor()
    {
        std::cout << "\033[?25l";
    }

    void show_cursor()
    {
        std::cout << "\033[?25h";
    }
}