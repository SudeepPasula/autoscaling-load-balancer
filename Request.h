/**
 * @file Request.h
 * @brief Defines the Request struct that represents a single web request.
 */

#ifndef REQUEST_H
#define REQUEST_H

#include <string>

/**
 * @struct Request
 * @brief A single web request handled by the load balancer.
 *
 * Holds the requester's IP, the destination IP, how many clock cycles the
 * request takes to process, and the job type ('P' for processing, 'S' for
 * streaming).
 */
struct Request {
    long id = 0;              ///< Unique, increasing request identifier.
    std::string ipIn;         ///< IP address of the requester.
    std::string ipOut;        ///< IP address the result is sent to.
    int time = 0;             ///< Clock cycles required to process the request.
    char jobType = 'P';       ///< 'P' = processing, 'S' = streaming.
    long arrivalCycle = 0;    ///< Clock cycle at which the request arrived.
};

#endif // REQUEST_H
