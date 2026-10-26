/**
 * @file Firewall.h
 * @brief Declares the Firewall class, an IP range blocker for DOS prevention.
 */

#ifndef FIREWALL_H
#define FIREWALL_H

#include <cstdint>
#include <string>
#include <vector>

/**
 * @struct IpRange
 * @brief An inclusive range of IPv4 addresses stored as 32-bit integers.
 */
struct IpRange {
    uint32_t start = 0;  ///< First address in the range.
    uint32_t end = 0;    ///< Last address in the range.
};

/**
 * @class Firewall
 * @brief Blocks requests whose source IP falls inside a blocked range.
 *
 * Rules can be written as CIDR ("10.0.0.0/8"), a range
 * ("192.168.1.0-192.168.1.255"), or a single address ("203.0.113.7").
 */
class Firewall {
public:
    /**
     * @brief Parses a rule and adds it to the block list.
     * @param rule Rule text.
     * @return true if the rule was valid.
     */
    bool addRule(const std::string& rule);

    /**
     * @brief Checks whether an IP address is blocked.
     * @param ip Dotted-quad IPv4 address.
     * @return true if the address is in a blocked range (or is malformed).
     */
    bool isBlocked(const std::string& ip) const;

    /**
     * @brief Gets all blocked ranges.
     * @return The list of ranges.
     */
    const std::vector<IpRange>& getRanges() const;

    /**
     * @brief Converts a dotted-quad string such as "192.168.0.1" to an integer.
     * @param ip The address text.
     * @param out Receives the converted value.
     * @return true if the address was valid.
     */
    static bool parseIp(const std::string& ip, uint32_t& out);

    /**
     * @brief Converts an integer address back to dotted-quad form.
     * @param ip The address as an integer.
     * @return The address text.
     */
    static std::string ipToString(uint32_t ip);

private:
    std::vector<IpRange> ranges_; ///< Blocked address ranges.
};

#endif // FIREWALL_H
