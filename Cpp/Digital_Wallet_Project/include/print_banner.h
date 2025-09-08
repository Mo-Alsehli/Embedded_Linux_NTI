#ifndef BANNER_PRINTER_H
#define BANNER_PRINTER_H

#include <iostream>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
inline int getTerminalWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return columns;
}
#else
#include <sys/ioctl.h>
#include <unistd.h>
inline int getTerminalWidth() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}
#endif

/// Print a single-line banner (centered message, full width)
inline void printBanner(const std::string &message, char fill = '=') {
    int width = getTerminalWidth();
    if (width <= 0) width = 80;  // fallback

    // Top border
    std::cout << "<" << std::string(width - 2, fill) << ">\n";

    // Message line (centered)
    int padding = (width - 2 - static_cast<int>(message.size()) - 2) / 2;
    if (padding < 0) padding = 0;

    std::cout << "<" << std::string(padding, fill) << " " << message << " "
              << std::string(width - 2 - padding - static_cast<int>(message.size()) - 2, fill) << ">\n";

    // Bottom border
    std::cout << "<" << std::string(width - 2, fill) << ">\n";
}

/// Print a multi-line banner (each message on its own line)
inline void printBanner(const std::vector<std::string> &messages, char fill = '=') {
    int width = getTerminalWidth();
    if (width <= 0) width = 80;  // fallback

    // Top border
    std::cout << "<" << std::string(width - 2, fill) << ">\n";

    for (const auto &message : messages) {
        int padding = (width - 2 - static_cast<int>(message.size()) - 2) / 2;
        if (padding < 0) padding = 0;

        std::cout << "<" << std::string(padding, fill) << " " << message << " "
                  << std::string(width - 2 - padding - static_cast<int>(message.size()) - 2, fill) << ">\n";
    }

    // Bottom border
    std::cout << "<" << std::string(width - 2, fill) << ">\n";
}

#endif  // BANNER_PRINTER_H
