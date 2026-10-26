/**
 * @file RequestGenerator.cpp
 * @brief Implements the RequestGenerator class.
 */

#include "RequestGenerator.h"

#include <utility>

RequestGenerator::RequestGenerator(const Config& config, std::vector<IpRange> attackRanges,
                                   unsigned int seed)
    : config_(config), attackRanges_(std::move(attackRanges)), rng_(seed), nextId_(1) {}

Request RequestGenerator::generate(long cycle) {
    std::uniform_real_distribution<double> chance(0.0, 1.0);
    std::uniform_int_distribution<int> taskTime(config_.minTaskTime, config_.maxTaskTime);

    Request request;
    request.id = nextId_++;
    request.arrivalCycle = cycle;
    request.time = taskTime(rng_);
    request.jobType = (chance(rng_) < config_.streamingProbability) ? 'S' : 'P';

    if (!attackRanges_.empty() && chance(rng_) < config_.attackProbability) {
        std::uniform_int_distribution<std::size_t> pick(0, attackRanges_.size() - 1);
        request.ipIn = randomIpInRange(attackRanges_[pick(rng_)]);
    } else {
        request.ipIn = randomIp();
    }
    request.ipOut = randomIp();
    return request;
}

int RequestGenerator::arrivalsThisCycle() {
    std::uniform_real_distribution<double> chance(0.0, 1.0);
    if (chance(rng_) >= config_.arrivalProbability) {
        return 0;
    }
    std::uniform_int_distribution<int> count(1, config_.maxArrivalsPerCycle);
    return count(rng_);
}

std::string RequestGenerator::randomIp() {
    std::uniform_int_distribution<int> first(1, 223);
    std::uniform_int_distribution<int> octet(0, 255);
    std::uniform_int_distribution<int> last(1, 254);
    return std::to_string(first(rng_)) + "." + std::to_string(octet(rng_)) + "." +
           std::to_string(octet(rng_)) + "." + std::to_string(last(rng_));
}

std::string RequestGenerator::randomIpInRange(const IpRange& range) {
    std::uniform_int_distribution<std::uint32_t> pick(range.start, range.end);
    return Firewall::ipToString(pick(rng_));
}
