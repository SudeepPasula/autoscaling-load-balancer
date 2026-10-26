/**
 * @file Switch.cpp
 * @brief Implements the Switch class.
 */

#include "Switch.h"

Switch::Switch(int processingServers, int streamingServers, const Config& config,
               const Firewall* firewall, Logger& logger)
    : processing_("LB-Processing", processingServers, config, firewall, logger),
      streaming_("LB-Streaming", streamingServers, config, firewall, logger),
      logger_(logger) {}

bool Switch::receiveRequest(const Request& request, long cycle) {
    if (request.jobType == 'S') {
        return streaming_.receiveRequest(request, cycle);
    }
    return processing_.receiveRequest(request, cycle);
}

void Switch::begin() {
    processing_.begin();
    streaming_.begin();
}

void Switch::tick(long cycle) {
    processing_.tick(cycle);
    streaming_.tick(cycle);
}

void Switch::printSummary(long cyclesRun) const {
    const LoadBalancerStats& p = processing_.getStats();
    const LoadBalancerStats& s = streaming_.getStats();

    logger_.print(EventType::Summary,
                  "\n===== Summary: Switch =====\n"
                  "Routed to processing:   " + std::to_string(p.received) + "\n"
                  "Routed to streaming:    " + std::to_string(s.received) + "\n"
                  "Total processed:        " + std::to_string(p.processed + s.processed) + "\n"
                  "Total blocked:          " + std::to_string(p.blocked + s.blocked) + "\n"
                  "Total servers at end:   " + std::to_string(processing_.activeServerCount() + streaming_.activeServerCount()));

    processing_.printSummary(cyclesRun);
    streaming_.printSummary(cyclesRun);
}
