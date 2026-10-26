/**
 * @file WebServer.cpp
 * @brief Implements the WebServer class.
 */

#include "WebServer.h"

WebServer::WebServer(int id) : id_(id), remainingTime_(0), draining_(false), processedCount_(0) {}

int WebServer::getId() const {
    return id_;
}

bool WebServer::isIdle() const {
    return remainingTime_ == 0;
}

void WebServer::assignRequest(const Request& request) {
    remainingTime_ = request.time;
}

bool WebServer::tick() {
    if (remainingTime_ == 0) {
        return false;
    }
    remainingTime_--;
    if (remainingTime_ == 0) {
        processedCount_++;
        return true;
    }
    return false;
}

long WebServer::getProcessedCount() const {
    return processedCount_;
}

void WebServer::startDraining() {
    draining_ = true;
}

bool WebServer::isDraining() const {
    return draining_;
}
