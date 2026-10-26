/**
 * @file LoadBalancer.cpp
 * @brief Implements the LoadBalancer class.
 */

#include "LoadBalancer.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

LoadBalancer::LoadBalancer(const std::string& name, int servers, const Config& config,
                           const Firewall* firewall, Logger& logger)
    : name_(name), config_(config), firewall_(firewall), logger_(logger), nextServerId_(1), nextScaleCheck_(0),
      blockedSinceReport_(0) {
    for (int i = 0; i < servers; i++) {
        servers_.push_back(new WebServer(nextServerId_++));
    }
}

LoadBalancer::~LoadBalancer() {
    for (WebServer* server : servers_) {
        delete server;
    }
}

bool LoadBalancer::receiveRequest(const Request& request, long cycle) {
    stats_.received++;
    if (firewall_ != nullptr && firewall_->isBlocked(request.ipIn)) {
        stats_.blocked++;
        blockedSinceReport_++;
        lastBlockedIp_ = request.ipIn;
        return false;
    }
    queue_.push(request);
    return true;
}

void LoadBalancer::begin() {
    stats_.startQueue = stats_.peakQueue = queue_.size();
    stats_.startServers = stats_.peakServers = activeServerCount();
    logger_.print(EventType::Summary, "[" + name_ + "] Starting with " + std::to_string(stats_.startServers) +
                                          " servers and " + std::to_string(stats_.startQueue) + " queued requests");
    reportBlocked(0);
}

void LoadBalancer::reportBlocked(long cycle) {
    if (blockedSinceReport_ == 0) {
        return;
    }
    logger_.event(EventType::Blocked, cycle, name_,
                  "Firewall rejected " + std::to_string(blockedSinceReport_) +
                      " requests from blocked IP ranges (latest from " + lastBlockedIp_ + ")");
    blockedSinceReport_ = 0;
}

void LoadBalancer::tick(long cycle) {
    // Idle servers ask for another request
    for (WebServer* server : servers_) {
        if (queue_.empty()) {
            break;
        }
        if (server->isIdle() && !server->isDraining()) {
            Request request = queue_.pop();
            stats_.totalWaitCycles += cycle - request.arrivalCycle;
            stats_.assigned++;
            server->assignRequest(request);
        }
    }

    // Process one cycle on every server, and free draining servers once they finish
    for (size_t i = 0; i < servers_.size();) {
        WebServer* server = servers_[i];
        stats_.totalServerCycles++;
        if (!server->isIdle()) {
            stats_.busyServerCycles++;
            if (server->tick()) {
                stats_.processed++;
            }
        }

        if (server->isDraining() && server->isIdle()) {
            logger_.event(EventType::ScaleDown, cycle, name_,
                          "Server #" + std::to_string(server->getId()) + " finished draining and was removed");
            delete server;
            servers_.erase(servers_.begin() + i);
        } else {
            i++;
        }
    }

    checkScaling(cycle);
    stats_.peakQueue = std::max(stats_.peakQueue, queue_.size());
    stats_.peakServers = std::max(stats_.peakServers, activeServerCount());

    if (config_.statusInterval > 0 && cycle % config_.statusInterval == 0) {
        reportBlocked(cycle);
        logger_.event(EventType::Status, cycle, name_,
                      "queue=" + std::to_string(queue_.size()) + " servers=" + std::to_string(activeServerCount()) +
                          " processed=" + std::to_string(stats_.processed) +
                          " blocked=" + std::to_string(stats_.blocked));
    }
}

void LoadBalancer::checkScaling(long cycle) {
    if (cycle < nextScaleCheck_) {
        return;
    }
    size_t servers = activeServerCount();
    size_t queued = queue_.size();

    if (queued > config_.upperThreshold * servers && servers < static_cast<size_t>(config_.maxServers)) {
        addServer(cycle);
        nextScaleCheck_ = cycle + config_.scaleCooldown;
    } else if (queued < config_.lowerThreshold * servers && servers > static_cast<size_t>(config_.minServers)) {
        removeServer(cycle);
        nextScaleCheck_ = cycle + config_.scaleCooldown;
    }
}

void LoadBalancer::addServer(long cycle) {
    int before = activeServerCount();
    WebServer* server = new WebServer(nextServerId_++);
    servers_.push_back(server);
    stats_.serversAdded++;

    logger_.event(EventType::ScaleUp, cycle, name_,
                  "Queue at " + std::to_string(queue_.size()) + ", added server #" + std::to_string(server->getId()) +
                      " (" + std::to_string(before) + " -> " + std::to_string(before + 1) + " servers)");
}

void LoadBalancer::removeServer(long cycle) {
    int before = activeServerCount();
    stats_.serversRemoved++;
    std::string change = " (" + std::to_string(before) + " -> " + std::to_string(before - 1) + " servers)";

    // Free an idle server right away if there is one
    for (size_t i = 0; i < servers_.size(); i++) {
        WebServer* server = servers_[i];
        if (server->isIdle() && !server->isDraining()) {
            logger_.event(EventType::ScaleDown, cycle, name_,
                          "Queue at " + std::to_string(queue_.size()) + ", removed idle server #" +
                              std::to_string(server->getId()) + change);
            delete server;
            servers_.erase(servers_.begin() + i);
            return;
        }
    }

    // Otherwise let a busy server finish its request first
    for (WebServer* server : servers_) {
        if (!server->isDraining()) {
            server->startDraining();
            logger_.event(EventType::ScaleDown, cycle, name_,
                          "Queue at " + std::to_string(queue_.size()) + ", draining server #" +
                              std::to_string(server->getId()) + change);
            return;
        }
    }
}

void LoadBalancer::printSummary(long cyclesRun) const {
    size_t servers = activeServerCount();
    size_t low = config_.lowerThreshold * servers;
    size_t high = config_.upperThreshold * servers;
    double avgWait = stats_.assigned > 0 ? static_cast<double>(stats_.totalWaitCycles) / stats_.assigned : 0;
    double utilization = stats_.totalServerCycles > 0 ? 100.0 * stats_.busyServerCycles / stats_.totalServerCycles : 0;

    std::string state = "balanced";
    if (queue_.size() > high) {
        state = "overloaded";
    } else if (queue_.size() < low) {
        state = "underutilized";
    }

    std::ostringstream out;
    out << std::fixed << std::setprecision(1)
        << "\n===== Summary: " << name_ << " =====\n"
        << "Cycles run:             " << cyclesRun << "\n"
        << "Task time range:        " << config_.minTaskTime << "-" << config_.maxTaskTime << " cycles\n"
        << "Queue size:             " << stats_.startQueue << " at start, " << queue_.size() << " at end, "
        << stats_.peakQueue << " peak\n"
        << "Requests received:      " << stats_.received << " (" << stats_.blocked << " blocked by firewall)\n"
        << "Requests processed:     " << stats_.processed << " (" << stats_.assigned - stats_.processed
        << " still in progress)\n"
        << "Average queue wait:     " << avgWait << " cycles\n"
        << "Servers:                " << stats_.startServers << " at start, " << servers << " at end, "
        << stats_.peakServers << " peak\n"
        << "Servers added/removed:  " << stats_.serversAdded << " / " << stats_.serversRemoved << "\n"
        << "Server utilization:     " << utilization << "%\n"
        << "Final queue target:     " << low << "-" << high << " (" << state << ")\n"
        << "Requests per server:";

    // Show how the work was spread across the servers still running
    for (size_t i = 0; i < servers_.size(); i++) {
        if (i % 6 == 0) {
            out << "\n ";
        }
        out << " #" << std::setw(2) << servers_[i]->getId() << ": " << std::setw(5) << servers_[i]->getProcessedCount();
    }
    logger_.print(EventType::Summary, out.str());
}

size_t LoadBalancer::queueSize() const {
    return queue_.size();
}

int LoadBalancer::activeServerCount() const {
    int count = 0;
    for (const WebServer* server : servers_) {
        if (!server->isDraining()) {
            count++;
        }
    }
    return count;
}

const LoadBalancerStats& LoadBalancer::getStats() const {
    return stats_;
}
