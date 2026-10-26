/**
 * @file LoadBalancer.h
 * @brief Declares the LoadBalancer class, which manages a request queue and a pool of web servers.
 */

#ifndef LOAD_BALANCER_H
#define LOAD_BALANCER_H

#include <string>
#include <vector>

#include "Config.h"
#include "Firewall.h"
#include "Logger.h"
#include "RequestQueue.h"
#include "WebServer.h"

/**
 * @struct LoadBalancerStats
 * @brief Counters a load balancer collects during a run.
 */
struct LoadBalancerStats {
    long received = 0;            ///< Requests that reached the load balancer.
    long blocked = 0;             ///< Requests the firewall rejected.
    long assigned = 0;            ///< Requests handed to a server.
    long processed = 0;           ///< Requests completed by servers.
    long serversAdded = 0;        ///< Scale-up actions.
    long serversRemoved = 0;      ///< Scale-down actions.
    long totalWaitCycles = 0;     ///< Total cycles requests spent in the queue.
    long busyServerCycles = 0;    ///< Cycles servers spent busy, summed over servers.
    long totalServerCycles = 0;   ///< Cycles servers existed, summed over servers.
    size_t startQueue = 0;        ///< Queue size when the run started.
    size_t peakQueue = 0;         ///< Largest queue size seen.
    int startServers = 0;         ///< Servers when the run started.
    int peakServers = 0;          ///< Most servers at once.
};

/**
 * @class LoadBalancer
 * @brief Gives queued requests to web servers and scales the server pool.
 *
 * Each clock cycle, idle servers take the next request from the queue, every
 * server advances one cycle, and the queue size is checked:
 *   - queue > upper_threshold * servers: add one server.
 *   - queue < lower_threshold * servers: remove one server.
 * After a scaling action, the next check waits scale_cooldown cycles.
 *
 * Servers are created with new and freed with delete. If every server is busy
 * when one needs to be removed, one is marked as draining and freed when its
 * request is done.
 */
class LoadBalancer {
public:
    /**
     * @brief Constructs a load balancer with its starting servers.
     * @param name Name shown in log output.
     * @param servers Starting number of servers.
     * @param config Simulation settings.
     * @param firewall Firewall for incoming requests, or nullptr for none.
     * @param logger Logger for events and summaries.
     */
    LoadBalancer(const std::string& name, int servers, const Config& config,
                 const Firewall* firewall, Logger& logger);

    /**
     * @brief Frees all remaining web servers.
     */
    ~LoadBalancer();

    LoadBalancer(const LoadBalancer&) = delete;
    LoadBalancer& operator=(const LoadBalancer&) = delete;

    /**
     * @brief Queues an incoming request unless the firewall blocks it.
     * @param request The incoming request.
     * @param cycle The current clock cycle.
     * @return true if the request was queued.
     */
    bool receiveRequest(const Request& request, long cycle);

    /**
     * @brief Records the starting state. Call after the initial queue is filled.
     */
    void begin();

    /**
     * @brief Runs one clock cycle: assign requests, process, then scale.
     * @param cycle The current clock cycle.
     */
    void tick(long cycle);

    /**
     * @brief Writes this load balancer's end-of-run summary.
     * @param cyclesRun Number of cycles simulated.
     */
    void printSummary(long cyclesRun) const;

    /**
     * @brief Gets the number of queued requests.
     * @return The queue size.
     */
    size_t queueSize() const;

    /**
     * @brief Counts servers that can take new requests (not draining).
     * @return Active server count.
     */
    int activeServerCount() const;

    /**
     * @brief Gets the counters collected so far.
     * @return The stats.
     */
    const LoadBalancerStats& getStats() const;

private:
    /**
     * @brief Applies the scaling rule if the cooldown has passed.
     * @param cycle The current clock cycle.
     */
    void checkScaling(long cycle);

    /**
     * @brief Allocates one new server.
     * @param cycle The current clock cycle.
     */
    void addServer(long cycle);

    /**
     * @brief Frees an idle server, or marks a busy one as draining.
     * @param cycle The current clock cycle.
     */
    void removeServer(long cycle);

    /**
     * @brief Logs how many requests the firewall rejected since the last report.
     * @param cycle The current clock cycle.
     */
    void reportBlocked(long cycle);

    std::string name_;                ///< Display name.
    const Config& config_;              ///< Simulation settings.
    const Firewall* firewall_;          ///< Firewall, or nullptr.
    Logger& logger_;                    ///< Output sink.
    RequestQueue queue_;                ///< Pending requests.
    std::vector<WebServer*> servers_;   ///< Allocated web servers.
    int nextServerId_;                  ///< ID for the next new server.
    long nextScaleCheck_;               ///< First cycle the next scaling check may run.
    long blockedSinceReport_;           ///< Requests blocked since the last blocked report.
    std::string lastBlockedIp_;         ///< Source IP of the most recent blocked request.
    LoadBalancerStats stats_;           ///< Collected counters.
};

#endif // LOAD_BALANCER_H
