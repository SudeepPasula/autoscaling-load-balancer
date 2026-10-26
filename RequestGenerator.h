/**
 * @file RequestGenerator.h
 * @brief Declares the RequestGenerator class, which creates random requests.
 */

#ifndef REQUEST_GENERATOR_H
#define REQUEST_GENERATOR_H

#include <random>
#include <string>
#include <vector>

#include "Config.h"
#include "Firewall.h"
#include "Request.h"

/**
 * @class RequestGenerator
 * @brief Makes random requests and decides how many arrive each cycle.
 *
 * A small fraction of requests can come from a blocked IP range. These
 * simulate attack traffic so the firewall has something to block.
 */
class RequestGenerator {
public:
    /**
     * @brief Constructs a generator.
     * @param config Simulation settings (task times, probabilities).
     * @param attackRanges Ranges that attack traffic comes from.
     * @param seed Random seed.
     */
    RequestGenerator(const Config& config, std::vector<IpRange> attackRanges, unsigned int seed);

    /**
     * @brief Creates one random request.
     * @param cycle The clock cycle when the request arrives.
     * @return The new request.
     */
    Request generate(long cycle);

    /**
     * @brief Picks how many new requests arrive in the current cycle.
     * @return 0 to max_arrivals_per_cycle.
     */
    int arrivalsThisCycle();

private:
    /**
     * @brief Creates a random public-looking IPv4 address.
     * @return Dotted-quad address.
     */
    std::string randomIp();

    /**
     * @brief Creates a random address inside a range.
     * @param range Range to pick from.
     * @return Dotted-quad address.
     */
    std::string randomIpInRange(const IpRange& range);

    const Config& config_;              ///< Simulation settings.
    std::vector<IpRange> attackRanges_; ///< Ranges for attack traffic.
    std::mt19937 rng_;                  ///< Random number engine.
    long nextId_;                       ///< ID for the next request.
};

#endif // REQUEST_GENERATOR_H
