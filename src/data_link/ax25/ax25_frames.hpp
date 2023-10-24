/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   ax25_frames.hpp
 * @brief  AX.25 Frame Definitions
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-01
 * @copyright  2023 (license to be defined)
 */

#ifndef AX25_FRAMES_HPP_
#define AX25_FRAMES_HPP_

#include <array>
#include <cstdint>
#include <vector>

namespace ax25 {
inline constexpr uint8_t kFlag = 0x7E;

/**
 * @brief AX.25 Address, seven bytes.
 */
typedef std::array<uint8_t, 7> address_t;

/**
 * @brief AX.25 Frame Check Sequence, CRC-CCITT, two bytes
 */
typedef uint16_t fcs_t;

/**
 * @brief AX.25 control field functions and types.
 */
namespace ctrl {
/**
 * @brief S S bits for supervisory frames.
 */
enum class SFrameSupervisoryBits : std::uint8_t {
  RECEIVE_READY = 0b00,     // RR - System Ready To Receive.
  RECEIVE_NOT_READY = 0b01, // RNR - TNC Buffer Full.
  REJECT = 0b10,            // RNR - Out of Sequence or Duplicate.
  SELECTIVE_REJECT = 0b11   // SREJ - Request single frame repeat.
};

/**
 * @brief M bits for unnumbered frames.
 * @details M M M | P/F | M   M | 1   1
 */
enum class UFrameControlField : std::uint8_t {
  SET_ASYNC_BALANCED_MODE_SABME = 0b01101100, // Connection Request
  SET_NORMAL_RESPONSE_MODE_SABM =
      0b00101100,               // Connection Request Extended (modulo 128)
  DISCONNECT = 0b01000000,      // Disconnect Request
  DISCONNECT_MODE = 0b00001100, // Disconnect Mode - System Busy or Disconnected
  UNNUMBERED_ACKNOWLEDGE = 0b01100000, // Unnumbered Acknowledge
  FRAME_REJECT = 0b10000100,           // Frame Reject
  UNNUMBERED_INFORMATION = 0b00000000, // Unnumbered Information
  EXCHANGE_IDENTIFICATION =
      0b10101100,   // Exchange Identification - Negotiate features
  TEST = 0b11100000 // Test
};

/**
 * @brief Control field format for modulo 8 I frames.
 * @details
 * 7 6 5 | 4 | 3 2 1 | 0
 *  N(R) | P |  N(S) | 0
 *
 * N(R) = Receive sequence number
 * P = Poll bit
 * N(S) = Send sequence number
 *
 * @param receive_sequence_number N(R) - Masked to the least significant 3 bits
 * and shifted over. (0-7 inclusive)
 * @param poll_bit P - true if the poll bit should be set, false otherwise.
 * @param send_sequence_number N(S) - Masked to the least significant 3 bits and
 * shifted over. (0-7 inclusive)
 *
 * @return uint8_t - The control field byte for the I frame.
 */
uint8_t IFrameControlFieldModulo8(uint8_t receive_sequence_number,
                                  bool poll_bit, uint8_t send_sequence_number);

/**
 * @brief Control field format for modulo 8 S frames.
 * @details
 * 7 6 5 |  4  | 3 | 2 | 1 | 0
 *  N(R) | P/F | S | S | 0   0
 *
 * N(R) = Receive sequence number
 * P/F = Poll/Final bit
 * S = Supervisory function bits.
 *
 * @param receive_sequence_number N(R) - Masked to the least significant 3 bits
 * and shifted over. (0-7 inclusive)
 * @param poll_final_bit P/F - true if the poll/final bit should be set, false
 * otherwise.
 * @param supervisory_bits S - Enum value for the supervisory bits.
 *
 * @return uint8_t - The control field byte for the S frame.
 */
uint8_t SFrameControlFieldModulo8(uint8_t receive_sequence_number,
                                  bool poll_final_bit,
                                  SFrameSupervisoryBits supervisory_bits);

/**
 * @brief Control field format for modulo 8 U frames.
 *
 *  7 6 5 |  4  | 3 | 2 | 1 | 0
 *  M M M | P/F | M   M | 1   1
 *
 * M = Unnumbered frame modifier bits
 * P/F = Poll/Final bit
 *
 * @param unnumbered_frame_modifier_bits M - Masked over the entire byte.
 * @param poll_final_bit P/F - true if the poll/final bit should be set.
 * @return uint8_t - The control field byte for the U frame.
 */
uint8_t UFrameControlFieldModulo8(uint8_t unnumbered_frame_modifier_bits,
                                  bool poll_final_bit);

/**
 * @brief Control field format for modulo 128 I frames.
 * @details The control field is two bytes:
 *
 * 15 14 13 12 11 10 9 | 8 | 7 6 5 4 3 2 1 | 0
 *        N(R)         | P |      N(S)     | 0
 *
 * N(R) = Receive sequence number
 * P = Poll bit
 * N(S) = Send sequence number
 *
 * @param receive_sequence_number N(R) - Masked to the least significant 7 bits
 * and shifted over. (0-127 inclusive)
 * @param poll_bit P - true if the poll bit should be set, false otherwise.
 * @param send_sequence_number N(S) - Masked to the least significant 7 bits and
 * shifted over. (0-127 inclusive)
 * @return uint16_t - The control field bytes for the I frame.
 */
uint16_t IFrameControlFieldModulo128(uint8_t receive_sequence_number,
                                     bool poll_bit,
                                     uint8_t send_sequence_number);

/**
 * @brief Control field format for modulo 8 S frames.
 * @details The control field is two bytes:
 *
 * 15 14 13 12 11 10 9 |  8  | 7 6 5 4 | 3 | 2 | 1 0
 *        N(R)         | P/F | 0 0 0 0 | S | S | 0 1
 *
 * N(R) = Receive sequence number
 * P/F = Poll/Final bit
 * S = Supervisory function bits.
 *
 * @param receive_sequence_number N(R) - Masked to the least significant 7 bits
 * and shifted over. (0-127 inclusive)
 * @param poll_final_bit P/F - true if the poll/final bit should be set, false
 * otherwise.
 * @param supervisory_bits S - Enum value for the supervisory bits.
 *
 * @return uint16_t - The control field bytes for the S frame.
 */
uint16_t SFrameControlFieldModulo128(uint8_t receive_sequence_number,
                                     bool poll_final_bit,
                                     SFrameSupervisoryBits supervisory_bits);
} // namespace ctrl
} // namespace ax25

#endif /* AX25_FRAMES_HPP_ */