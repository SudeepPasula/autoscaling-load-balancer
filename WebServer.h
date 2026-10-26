/**
 * @file WebServer.h
 * @brief Declares the WebServer class, which processes one request at a time.
 */

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "Request.h"

/**
 * @class WebServer
 * @brief Simulated web server that processes a single request at a time.
 *
 * The load balancer hands the server a request. The server counts down the
 * request's processing time one clock cycle at a time, and when it finishes
 * it is idle and ready for another request.
 *
 * A server marked as "draining" finishes its current request but gets no new
 * ones. The load balancer deletes it once it goes idle.
 */
class WebServer {
public:
    /**
     * @brief Constructs an idle web server.
     * @param id Identifier unique within the owning load balancer.
     */
    explicit WebServer(int id);

    /**
     * @brief Gets the server's identifier.
     * @return The server ID.
     */
    int getId() const;

    /**
     * @brief Checks whether the server has no request in progress.
     * @return true if idle.
     */
    bool isIdle() const;

    /**
     * @brief Starts processing a new request.
     * @param request The request to process.
     */
    void assignRequest(const Request& request);

    /**
     * @brief Advances the server by one clock cycle.
     * @return true if the current request finished during this cycle.
     */
    bool tick();

    /**
     * @brief Gets how many requests this server has completed.
     * @return Completed request count.
     */
    long getProcessedCount() const;

    /**
     * @brief Marks the server for removal once its current request is done.
     */
    void startDraining();

    /**
     * @brief Checks whether the server is waiting to be removed.
     * @return true if draining.
     */
    bool isDraining() const;

private:
    int id_;                ///< Server identifier.
    int remainingTime_;     ///< Cycles left on the current request (0 = idle).
    bool draining_;         ///< Whether the server is scheduled for removal.
    long processedCount_;   ///< Number of requests completed.
};

#endif // WEB_SERVER_H
