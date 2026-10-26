/**
 * @file Config.h
 * @brief Declares the Config struct and the configuration file loader.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

/**
 * @struct Config
 * @brief All tunable simulation settings.
 *
 * Most fields can be set in the configuration file (see config.txt). The
 * number of servers and clock cycles are entered by the user at startup.
 */
struct Config {
    int numServers = 10;                ///< Starting number of web servers (user input).
    long runCycles = 10000;             ///< Number of clock cycles to simulate (user input).
    int initialQueueMultiplier = 100;   ///< Starting queue = servers * this value.

    int minTaskTime = 3;                ///< Shortest request processing time (cycles).
    int maxTaskTime = 100;              ///< Longest request processing time (cycles).
    double arrivalProbability = 0.25;   ///< Chance that new requests arrive in a cycle.
    int maxArrivalsPerCycle = 3;        ///< Most new requests that can arrive in one cycle.

    int lowerThreshold = 50;            ///< Remove a server when queue < lower * servers.
    int upperThreshold = 80;            ///< Add a server when queue > upper * servers.
    int scaleCooldown = 50;             ///< Cycles to wait after a scaling action.
    int minServers = 1;                 ///< Fewest servers a load balancer may have.
    int maxServers = 1000;              ///< Most servers a load balancer may have.

    double streamingProbability = 0.3;  ///< Fraction of jobs that are streaming ('S').
    bool useSwitch = false;             ///< Route P and S jobs to separate load balancers.

    std::vector<std::string> blockedRanges; ///< Firewall rules.
    double attackProbability = 0.02;    ///< Chance a request comes from a blocked range.

    bool verbose = true;                ///< Verbose event log vs. summaries only.
    std::string logFile = "loadbalancer.log"; ///< Path of the log file.
    long statusInterval = 5000;         ///< Cycles between status lines in verbose mode.
    unsigned int seed = 0;              ///< Random seed; 0 uses the current time.
};

/**
 * @brief Loads settings from a "key = value" configuration file.
 *
 * Text after '#' is a comment. Unknown keys and bad values are reported and
 * skipped.
 *
 * @param path Path of the configuration file.
 * @param config Settings to update. Fields not in the file keep their defaults.
 * @return true if the file could be opened.
 */
bool loadConfig(const std::string& path, Config& config);

#endif // CONFIG_H
