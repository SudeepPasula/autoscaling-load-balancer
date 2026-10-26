/**
 * @file Switch.h
 * @brief Declares the Switch class, which routes requests to load balancers by job type.
 */

#ifndef SWITCH_H
#define SWITCH_H

#include "Config.h"
#include "Firewall.h"
#include "LoadBalancer.h"
#include "Logger.h"

/**
 * @class Switch
 * @brief Higher-level router that sorts jobs by type (bonus feature).
 *
 * Streaming ('S') jobs go to one load balancer and processing ('P') jobs go
 * to another. Each load balancer has its own servers, queue, scaling, and
 * statistics. Both run in the same clock loop.
 */
class Switch {
public:
    /**
     * @brief Constructs the switch and its two load balancers.
     * @param processingServers Starting servers for the processing load balancer.
     * @param streamingServers Starting servers for the streaming load balancer.
     * @param config Simulation settings. Must outlive the switch.
     * @param firewall Firewall shared by both load balancers, or nullptr.
     * @param logger Logger for events and summaries.
     */
    Switch(int processingServers, int streamingServers, const Config& config,
           const Firewall* firewall, Logger& logger);

    /**
     * @brief Sends a request to the load balancer for its job type.
     * @param request The incoming request.
     * @param cycle The current clock cycle.
     * @return true if the target load balancer accepted it.
     */
    bool receiveRequest(const Request& request, long cycle);

    /**
     * @brief Records the starting state of both load balancers.
     */
    void begin();

    /**
     * @brief Runs one clock cycle on both load balancers.
     * @param cycle The current clock cycle.
     */
    void tick(long cycle);

    /**
     * @brief Writes combined totals, then each load balancer's summary.
     * @param cyclesRun Number of cycles simulated.
     */
    void printSummary(long cyclesRun) const;

private:
    LoadBalancer processing_;   ///< Handles 'P' jobs.
    LoadBalancer streaming_;    ///< Handles 'S' jobs.
    Logger& logger_;            ///< Output sink.
};

#endif // SWITCH_H
