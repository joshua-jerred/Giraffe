/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_constants.hpp
 * @brief  Constants for Giraffe Data Link
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-18
 * @copyright  2024 (license to be defined)
 */

#ifndef GDL_CONSTANTS_HPP_
#define GDL_CONSTANTS_HPP_

#include <cstdint>
#include <cstdlib>

namespace giraffe::gdl {
inline constexpr static int GDL_THREAD_SLEEP_INTERVAL_MS = 100;

inline constexpr static size_t GDL_MESSAGE_DATA_MAX_SIZE = 150;

inline constexpr static size_t GDL_MESSAGE_QUEUES_SIZE = 10;

/// @brief delay between sending a new transmission
inline constexpr static uint32_t GDL_NEW_TX_DELAY_MS = 100;

inline constexpr static uint8_t GDL_RDT_MAX_RETRIES = 5;
inline constexpr static uint32_t GDL_RDT_RETRY_INTERVAL_MS = 10000;
inline constexpr static uint32_t GDL_CONNECTION_STATUS_INTERVAL_MS = 120 * 1000;

inline constexpr static uint32_t GDL_EXCHANGE_CONNECTION_TIMEOUT_MS =
    200 * 1000;

/// @brief If we don't have an exchange message within this time, we will send a
/// ping to check the connection.
inline constexpr static uint32_t GDL_EXCHANGE_PING_INTERVAL_MS = 60 * 1000;
// GDL_EXCHANGE_CONNECTION_TIMEOUT_MS / 2;

inline constexpr static uint32_t GDL_DOWNLINK_TIMEOUT_MS = 200 * 1000;
inline constexpr static uint32_t GDL_UPLINK_TIMEOUT_MS = 200 * 1000;

} // namespace giraffe::gdl

#endif /* GDL_CONSTANTS_HPP_ */