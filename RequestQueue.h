/**
 * @file RequestQueue.h
 * @brief Declares the RequestQueue class, a FIFO queue of Request objects.
 */

#ifndef REQUEST_QUEUE_H
#define REQUEST_QUEUE_H

#include <cstddef>
#include <queue>

#include "Request.h"

/**
 * @class RequestQueue
 * @brief First-in, first-out queue of pending requests.
 *
 * Thin wrapper around std::queue that gives the load balancer a clear,
 * request-specific interface.
 */
class RequestQueue {
public:
    /**
     * @brief Adds a request to the back of the queue.
     * @param request The request to enqueue.
     */
    void push(const Request& request);

    /**
     * @brief Removes and returns the request at the front of the queue.
     * @pre The queue is not empty.
     * @return The request that was at the front.
     */
    Request pop();

    /**
     * @brief Checks whether the queue has no requests.
     * @return true if the queue is empty.
     */
    bool empty() const;

    /**
     * @brief Gets the number of requests waiting in the queue.
     * @return The queue size.
     */
    std::size_t size() const;

private:
    std::queue<Request> requests_; ///< Underlying FIFO storage.
};

#endif // REQUEST_QUEUE_H
