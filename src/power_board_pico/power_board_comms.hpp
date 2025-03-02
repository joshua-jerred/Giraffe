/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   power_board_comms.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2025-03-01
/// @copyright  2025 (license to be defined)

#pragma once

#include "pico/printf.h"
#include "pico/stdlib.h"

#include <array>
#include <cstdint>
#include <span>
#include <string>

namespace power_board {

class PowerBoardComms {
public:
  PowerBoardComms() {
    stdio_init_all();
    stdio_usb_init();
    // stdio_set_translate_crlf(stdio_uart, true);
  }

  // ~PowerBoardComms();

  void process() {
    // No need to do this with IRQs yet

    uint8_t c = stdio_getchar_timeout_us(100000);
    if (c != PICO_ERROR_TIMEOUT && c < 128) {
      processChar(c);
      //   // convert to decimal number
      //   printf("%d\n", c);
      //   // stdio_putchar(c); // echo
    }

    // stdio_flush();
  }

  void charsAvailable() {
    // No need to do this with IRQs yet
    volatile uint8_t c = stdio_getchar_timeout_us(100);
    if (c != PICO_ERROR_TIMEOUT) {
      processChar(c);
    }
  }

private:
  void processChar(uint8_t c) {
    // Ensure we don't overflow the buffer
    if (rx_buffer_index_ >= rx_buffer_.size()) {
      rx_buffer_index_ = 0;
      sendError("Buffer overflow");
    }

    // stdio_putchar(c); // echo

    // End of line
    if (c == '\n' || c == '\r') {
      if (rx_buffer_index_ == 0) { // ignore extra newlines/linefeeds
        return;
      }

      rx_buffer_[rx_buffer_index_] = 0; // null terminate
      processLine(std::span(rx_buffer_.data(), rx_buffer_index_));
      rx_buffer_index_ = 0;

      return;
    }

    rx_buffer_[rx_buffer_index_++] = c;
  }

  void processLine(std::span<uint8_t> line) {
    const std::array<uint8_t, 4> cmd_start_sequence = {'c', 'm', 'd', ' '};
    size_t cmd_start_index = 0;
    for (size_t i = 0; i < line.size(); i++) {
      if (line[i] == cmd_start_sequence.at(cmd_start_index)) {
        cmd_start_index++;
        if (cmd_start_index == cmd_start_sequence.size()) {
          processCommand(line.subspan(i + 1));
          return;
        }
      } else {
        cmd_start_index = 0;
      }
    }

    sendError("unknown format");
  }

  void processCommand(std::span<uint8_t> command) {
    std::string command_str(command.begin(), command.end());

    // split input into a command and optional argument
    std::string_view arg;
    size_t space_index = command_str.find(' ');
    if (space_index != std::string::npos) {
      arg = command_str.substr(space_index + 1);
      command_str = command_str.substr(0, space_index);
    }

    if (command_str == "ping") {
      acknowledge();
      return;
    } else if (command_str == "version") {
      const std::string version_number = GIRAFFE_VERSION_NUMBER;
      const std::string version_stage = GIRAFFE_VERSION_STAGE;
      send(version_number + "-" + version_stage);
      return;
    }

    sendError("unknown cmd<" + command_str + ">");
  }

  void sendError(std::string_view error) {
    std::array<uint8_t, 32> buffer;
    buffer[0] = 0x15; // ASCII NAK
    buffer[1] = 'E';
    buffer[2] = 'R';
    buffer[3] = 'R';
    buffer[4] = ' ';
    const size_t start = 5;
    size_t i = start;
    for (; i < error.size() + start && i < buffer.size(); i++) {
      buffer.at(i) = error.at(i - start);
    }
    send(std::span(buffer.data(), i));
  }

  void acknowledge() {
    std::array<uint8_t, 4> buffer = {0x06, 'A', 'C', 'K'};
    send(std::span(buffer));
  }

  void send(std::span<uint8_t> data) {
    for (uint8_t c : data) {
      stdio_putchar(c);
    }
    stdio_putchar('\n');
  }

  void send(std::string_view data) {
    for (uint8_t c : data) {
      stdio_putchar(c);
    }
    stdio_putchar('\n');
  }

  std::array<uint8_t, 32> rx_buffer_;
  uint8_t rx_buffer_index_ = 0;
};

} // namespace power_board