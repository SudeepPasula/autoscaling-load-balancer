/**
 * @file Logger.cpp
 * @brief Implements the Logger class.
 */

#include "Logger.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include <unistd.h>

/**
 * @brief Gets the ANSI color code and tag for an event kind.
 * @param type Event kind.
 * @param tag Receives the tag shown in event lines.
 * @return The ANSI color escape sequence.
 */
static const char* style(EventType type, const char*& tag) {
    switch (type) {
        case EventType::ScaleUp:   tag = "SCALE UP"; return "\033[32m";
        case EventType::ScaleDown: tag = "SCALE DN"; return "\033[33m";
        case EventType::Blocked:   tag = "BLOCKED";  return "\033[31m";
        case EventType::Status:    tag = "STATUS";   return "\033[34m";
        case EventType::Warning:   tag = "WARNING";  return "\033[35m";
        case EventType::Summary:   tag = "SUMMARY";  return "\033[1;37m";
        default:                   tag = "INFO";     return "\033[36m";
    }
}

Logger::Logger(const std::string& path, bool verbose)
    : file_(path), verbose_(verbose), color_(isatty(STDOUT_FILENO)) {}

void Logger::event(EventType type, long cycle, const std::string& source, const std::string& message) {
    if (!verbose_) {
        return;
    }
    const char* tag = nullptr;
    style(type, tag);

    std::ostringstream line;
    line << "[cycle " << std::setw(7) << cycle << "] [" << source << "] "
         << std::left << std::setw(10) << tag << message;
    print(type, line.str());
}

void Logger::print(EventType type, const std::string& message) {
    const char* tag = nullptr;
    const char* color = style(type, tag);

    if (color_) {
        std::cout << color << message << "\033[0m\n";
    } else {
        std::cout << message << "\n";
    }
    file_ << message << "\n";
}
