/**
 * @file Firewall.cpp
 * @brief Implements the Firewall class.
 */

#include "Firewall.h"

#include <cstdio>
#include <cstdlib>

bool Firewall::parseIp(const std::string& ip, uint32_t& out) {
    int a, b, c, d;
    if (std::sscanf(ip.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d) != 4) {
        return false;
    }
    for (int part : {a, b, c, d}) {
        if (part < 0 || part > 255) {
            return false;
        }
    }
    out = (static_cast<uint32_t>(a) << 24) | (b << 16) | (c << 8) | d;
    return true;
}

std::string Firewall::ipToString(uint32_t ip) {
    return std::to_string(ip >> 24) + "." + std::to_string((ip >> 16) & 255) + "." +
           std::to_string((ip >> 8) & 255) + "." + std::to_string(ip & 255);
}

bool Firewall::addRule(const std::string& rule) {
    IpRange range;
    size_t slash = rule.find('/');
    size_t dash = rule.find('-');

    if (slash != std::string::npos) {
        // CIDR: keep the first "bits" bits of the address fixed
        int bits = std::atoi(rule.c_str() + slash + 1);
        if (!parseIp(rule.substr(0, slash), range.start) || bits < 0 || bits > 32) {
            return false;
        }
        uint32_t mask = bits == 0 ? 0 : 0xFFFFFFFFu << (32 - bits);
        range.start &= mask;
        range.end = range.start | ~mask;
    } else if (dash != std::string::npos) {
        if (!parseIp(rule.substr(0, dash), range.start) || !parseIp(rule.substr(dash + 1), range.end)) {
            return false;
        }
    } else {
        if (!parseIp(rule, range.start)) {
            return false;
        }
        range.end = range.start;
    }

    ranges_.push_back(range);
    return true;
}

bool Firewall::isBlocked(const std::string& ip) const {
    uint32_t value;
    if (!parseIp(ip, value)) {
        return true;
    }
    for (const IpRange& range : ranges_) {
        if (value >= range.start && value <= range.end) {
            return true;
        }
    }
    return false;
}

const std::vector<IpRange>& Firewall::getRanges() const {
    return ranges_;
}
