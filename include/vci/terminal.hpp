#pragma once

namespace vci
{
    struct TerminalSize
    {
        int rows;
        int cols;
    };

    TerminalSize get_terminal_size();
    void clear_screen();
    void hide_cursor();
    void show_cursor();
}
