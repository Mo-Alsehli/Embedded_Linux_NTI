#ifndef PRINT_MESSAGE_H
#define PRINT_MESSAGE_H

#include <iostream>
#include <string>
#include <vector>

enum class MsgType { INFO, WARNING, ERROR, SUCCESS };

inline void printMessage(const std::string& text, MsgType type = MsgType::INFO) {
    std::string prefix;
    switch (type) {
        case MsgType::INFO:
            prefix = "[INFO]    ";
            break;
        case MsgType::WARNING:
            prefix = "[WARNING] ";
            break;
        case MsgType::ERROR:
            prefix = "[ERROR]   ";
            break;
        case MsgType::SUCCESS:
            prefix = "[SUCCESS] ";
            break;
    }

    // pick a line width
    int width = 70;
    std::string line(width, '-');

    std::cout << line << "\n";
    std::cout << prefix << text << "\n";
    std::cout << line << "\n";
}

inline void printMessages(const std::vector<std::string>& texts, MsgType type = MsgType::INFO) {
    std::string prefix;
    switch (type) {
        case MsgType::INFO:
            prefix = "[INFO]    ";
            break;
        case MsgType::WARNING:
            prefix = "[WARNING] ";
            break;
        case MsgType::ERROR:
            prefix = "[ERROR]   ";
            break;
        case MsgType::SUCCESS:
            prefix = "[SUCCESS] ";
            break;
    }

    int width = 70;
    std::string line(width, '-');

    std::cout << line << "\n";
    for (auto& text : texts) {
        std::cout << prefix << text << "\n";
    }
    std::cout << line << "\n";
}

#endif  // PRINT_MESSAGE_H
