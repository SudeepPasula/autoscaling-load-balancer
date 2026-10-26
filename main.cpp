/**
 * @file main.cpp
 * @brief Driver program for the load balancer simulation.
 *
 * @mainpage Autoscaling Load Balancer
 *
 * Simulates a load balancer that hands web requests to a pool of web servers,
 * adding and removing servers so the queue stays between 50 and 80 requests
 * per server. Settings are read from config.txt, and the number of servers and
 * clock cycles are entered by the user at startup.
 *
 * Usage: ./loadbalancer [config file] [-v | -q]
 */

#include <algorithm>
#include <ctime>
#include <iostream>
#include <string>

#include "Config.h"
#include "Firewall.h"
#include "LoadBalancer.h"
#include "Logger.h"
#include "RequestGenerator.h"
#include "Switch.h"

/**
 * @brief Fills the starting queue, runs the clock, and prints the summary.
 * @tparam Balancer Either a LoadBalancer or a Switch.
 * @param balancer Receives and processes the requests.
 * @param generator Makes the random requests.
 * @param config Simulation settings.
 */
template <typename Balancer>
void simulate(Balancer& balancer, RequestGenerator& generator, const Config& config) {
    // Start with a full queue
    long initialRequests = static_cast<long>(config.numServers) * config.initialQueueMultiplier;
    for (long i = 0; i < initialRequests; i++) {
        balancer.receiveRequest(generator.generate(0), 0);
    }
    balancer.begin();

    for (long cycle = 1; cycle <= config.runCycles; cycle++) {
        // New requests show up at random times
        int arrivals = generator.arrivalsThisCycle();
        for (int i = 0; i < arrivals; i++) {
            balancer.receiveRequest(generator.generate(cycle), cycle);
        }
        balancer.tick(cycle);
    }

    balancer.printSummary(config.runCycles);
}

/**
 * @brief Program entry point.
 * @param argc Argument count.
 * @param argv Optional config file path and -v (verbose) or -q (quiet).
 * @return 0 on success, 1 on bad input.
 */
int main(int argc, char* argv[]) {
    std::string configFile = "config.txt";
    std::string mode;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "-q") {
            mode = arg;
        } else {
            configFile = arg;
        }
    }

    Config config;
    if (!loadConfig(configFile, config)) {
        std::cout << "Could not open " << configFile << ", using default settings.\n";
    }
    if (mode == "-v") config.verbose = true;
    if (mode == "-q") config.verbose = false;

    std::cout << "Number of web servers: ";
    std::cin >> config.numServers;
    std::cout << "Clock cycles to run: ";
    std::cin >> config.runCycles;
    if (!std::cin || config.numServers < 1 || config.runCycles < 1) {
        std::cerr << "Servers and clock cycles must be positive numbers.\n";
        return 1;
    }

    Logger logger(config.logFile, config.verbose);

    Firewall firewall;
    for (const std::string& rule : config.blockedRanges) {
        if (!firewall.addRule(rule)) {
            logger.print(EventType::Warning, "Skipping bad firewall rule: " + rule);
        }
    }

    unsigned int seed = config.seed != 0 ? config.seed : static_cast<unsigned int>(time(nullptr));
    RequestGenerator generator(config, firewall.getRanges(), seed);

    logger.print(EventType::Summary, "===== Load Balancer Configuration =====");
    logger.print(EventType::Info, std::string("Mode: ") + (config.useSwitch ? "switch (separate P and S load balancers)" : "single load balancer") +
                                      ", " + (config.verbose ? "verbose" : "quiet"));
    logger.print(EventType::Info, "Servers: " + std::to_string(config.numServers) +
                                      ", clock cycles: " + std::to_string(config.runCycles) +
                                      ", seed: " + std::to_string(seed));
    logger.print(EventType::Info, "Initial queue: " + std::to_string(config.numServers * config.initialQueueMultiplier) +
                                      ", task time: " + std::to_string(config.minTaskTime) + "-" +
                                      std::to_string(config.maxTaskTime) + " cycles");
    logger.print(EventType::Info, "New requests: up to " + std::to_string(config.maxArrivalsPerCycle) + " per cycle, " +
                                      std::to_string(static_cast<int>(config.arrivalProbability * 100)) + "% of cycles");
    logger.print(EventType::Info, "Queue target: " + std::to_string(config.lowerThreshold) + "-" +
                                      std::to_string(config.upperThreshold) + " requests per server, " +
                                      std::to_string(config.scaleCooldown) + " cycle cooldown");
    for (const std::string& rule : config.blockedRanges) {
        logger.print(EventType::Blocked, "Firewall blocks " + rule);
    }
    if (config.verbose) {
        logger.print(EventType::Summary, "\n===== Events =====");
    }

    if (config.useSwitch) {
        // Split the servers between the two load balancers based on the expected job mix
        int streamingServers = std::max(1, static_cast<int>(config.numServers * config.streamingProbability));
        int processingServers = std::max(1, config.numServers - streamingServers);
        Switch jobSwitch(processingServers, streamingServers, config, &firewall, logger);
        simulate(jobSwitch, generator, config);
    } else {
        LoadBalancer balancer("LB-Main", config.numServers, config, &firewall, logger);
        simulate(balancer, generator, config);
    }

    return 0;
}
