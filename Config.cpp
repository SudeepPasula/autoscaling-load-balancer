/**
 * @file Config.cpp
 * @brief Implements the configuration file loader.
 */

#include "Config.h"

#include <fstream>
#include <iostream>

/**
 * @brief Removes leading and trailing whitespace.
 * @param s Input string.
 * @return The trimmed string.
 */
static std::string trim(const std::string& s) {
    size_t first = s.find_first_not_of(" \t\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = s.find_last_not_of(" \t\r");
    return s.substr(first, last - first + 1);
}

bool loadConfig(const std::string& path, Config& config) {
    std::ifstream file(path);
    if (!file) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = line.substr(0, line.find('#'));
        size_t eq = line.find('=');
        if (eq == std::string::npos) {
            continue;
        }
        std::string key = trim(line.substr(0, eq));
        std::string value = trim(line.substr(eq + 1));

        try {
            if (key == "initial_queue_multiplier") config.initialQueueMultiplier = std::stoi(value);
            else if (key == "min_task_time") config.minTaskTime = std::stoi(value);
            else if (key == "max_task_time") config.maxTaskTime = std::stoi(value);
            else if (key == "arrival_probability") config.arrivalProbability = std::stod(value);
            else if (key == "max_arrivals_per_cycle") config.maxArrivalsPerCycle = std::stoi(value);
            else if (key == "lower_threshold") config.lowerThreshold = std::stoi(value);
            else if (key == "upper_threshold") config.upperThreshold = std::stoi(value);
            else if (key == "scale_cooldown") config.scaleCooldown = std::stoi(value);
            else if (key == "min_servers") config.minServers = std::stoi(value);
            else if (key == "max_servers") config.maxServers = std::stoi(value);
            else if (key == "streaming_probability") config.streamingProbability = std::stod(value);
            else if (key == "use_switch") config.useSwitch = (value == "true");
            else if (key == "blocked_range") config.blockedRanges.push_back(value);
            else if (key == "attack_probability") config.attackProbability = std::stod(value);
            else if (key == "verbose") config.verbose = (value == "true");
            else if (key == "log_file") config.logFile = value;
            else if (key == "status_interval") config.statusInterval = std::stol(value);
            else if (key == "seed") config.seed = static_cast<unsigned int>(std::stoul(value));
            else std::cerr << "Unknown config setting: " << key << "\n";
        } catch (...) {
            std::cerr << "Bad value for " << key << ": " << value << "\n";
        }
    }
    return true;
}
