/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   to_string.hpp
 * @brief  Generic to_string functions for a variety of types.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 */

#ifndef TO_STRING_HPP_
#define TO_STRING_HPP_

#include "streams.hpp"

/**
 * @brief Generic to_string functions for a variety of types.
 * @details The only requirement is that they must be exception safe. So if they
 * use a map, first map sure the key exists.
 */
namespace util {
std::string to_string(const node::Identification id) noexcept;
std::string to_string(const DiagnosticId diagnostic_id) noexcept;
std::string to_string(data::DataPacket::Type type) noexcept;

std::string to_string(const data::DataId id) noexcept;
std::string to_string(const data::DataPacket &packet) noexcept;

std::string to_string(const data::LogPacket::Level level) noexcept;
std::string to_string(const data::LogPacket &packet) noexcept;

std::string to_string(const data::GpsFix fix_type) noexcept;
std::string to_string(const data::GpsFrame &frame) noexcept;
std::string to_string(const data::GpsFramePacket &packet) noexcept;

std::string to_string(const data::ImuFrame &frame) noexcept;
std::string to_string(const data::ImuFramePacket &packet) noexcept;
} // namespace util

#endif /* TO_STRING_HPP_ */