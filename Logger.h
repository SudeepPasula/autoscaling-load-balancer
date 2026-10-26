/**
 * @file Logger.h
 * @brief Declares the Logger class, which writes colored console output and a log file.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

/**
 * @enum EventType
 * @brief Kinds of events. Each kind has its own console color.
 */
enum class EventType {
    Info,       ///< General information (cyan).
    ScaleUp,    ///< A server was added (green).
    ScaleDown,  ///< A server was removed (yellow).
    Blocked,    ///< The firewall rejected a request (red).
    Status,     ///< Periodic status report (blue).
    Warning,    ///< A recoverable problem (magenta).
    Summary     ///< Start and end summaries (bold white).
};

/**
 * @class Logger
 * @brief Writes events to the console in color and to a log file without color.
 *
 * In verbose mode, event() writes changes as they happen. In quiet mode,
 * event() writes nothing, and only print() output (the summaries) appears.
 */
class Logger {
public:
    /**
     * @brief Opens the log file.
     * @param path Log file path.
     * @param verbose true to write per-event lines.
     */
    Logger(const std::string& path, bool verbose);

    /**
     * @brief Writes a tagged event line (verbose mode only).
     * @param type Event kind.
     * @param cycle Clock cycle when the event happened.
     * @param source Name of the component that raised the event.
     * @param message Event description.
     */
    void event(EventType type, long cycle, const std::string& source, const std::string& message);

    /**
     * @brief Writes a message in any mode.
     * @param type Event kind; sets the color.
     * @param message Text to write.
     */
    void print(EventType type, const std::string& message);

private:
    std::ofstream file_;  ///< Log file stream.
    bool verbose_;        ///< Verbose vs. quiet mode.
    bool color_;          ///< Whether the console supports color.
};

#endif // LOGGER_H
