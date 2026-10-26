/**
 * @file RequestQueue.cpp
 * @brief Implements the RequestQueue class.
 */

#include "RequestQueue.h"

void RequestQueue::push(const Request& request) {
    requests_.push(request);
}

Request RequestQueue::pop() {
    Request front = requests_.front();
    requests_.pop();
    return front;
}

bool RequestQueue::empty() const {
    return requests_.empty();
}

std::size_t RequestQueue::size() const {
    return requests_.size();
}
