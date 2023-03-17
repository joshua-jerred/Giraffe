/**
 * @file serial.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief A simple linux serial wrapper for extensions.
 * @date 2023-03-17
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <string>

#include "extension-interface.h"

/*
class Serial {
 public:
  Serial(std::string device_path, int baud_rate);
  ~Serial();

  int Connect();
  int Disconnect();

  std::string Read();
  int Write(std::string data);

 private:
  std::string device_path_;
  int baud_rate_;
  int fd_;
  bool connected_;
};
*/

interface::Serial::Serial(std::string device_path, Serial::BaudRate baud_rate,
                          int timeout)
    : device_path_(device_path), baud_rate_(baud_rate), timeout_(timeout) {
  fd_ = -1;
  connected_ = false;
}

interface::Serial::~Serial() {
  if (connected_) {
    Disconnect();
  }
}

void interface::Serial::Connect() {
  if (connected_) {
    throw interface::SerialException(
        interface::SerialException::Type::ALREADY_CONNECTED, 0,
        "Can not connect, already connected.");
  }

  fd_ = open(device_path_.c_str(), O_RDWR);

  if (fd_ < 0) {  // Error opening device
    throw interface::SerialException(
        interface::SerialException::Type::OPEN, errno,
        "Error opening device." + std::string(strerror(errno)));
  }

  struct termios tty;

  if (tcgetattr(fd_, &tty) != 0) {  // Error getting attributes}
    throw interface::SerialException(
        interface::SerialException::Type::GET_ATTRIBUTE, errno,
        "Error getting attributes." + std::string(strerror(errno)));
  }

  // The following values are generic, this class can be changed in the future
  // based upon the needs of an extension.
  tty.c_cflag &= ~PARENB;         // No parity
  tty.c_cflag &= ~CSTOPB;         // 1 stop bit
  tty.c_cflag &= ~CSIZE;          // Clear size bits
  tty.c_cflag |= CS8;             // 8 data bits
  tty.c_cflag &= ~CRTSCTS;        // No flow control
  tty.c_cflag |= CREAD | CLOCAL;  // Enable read, ignore control lines

  tty.c_lflag &= ~ICANON;  // Disable canonical mode
  tty.c_lflag &= ~ECHO;    // Disable echo
  tty.c_lflag &= ~ECHOE;   // Disable erasure
  tty.c_lflag &= ~ECHONL;  // Disable new-line echo
  tty.c_lflag &= ~ISIG;    // No interpretation

  tty.c_iflag &= ~(IXON | IXOFF | IXANY);  // No software flow control
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                   ICRNL);  // Disable special handling

  tty.c_oflag &= ~OPOST;  // Disable output processing
  tty.c_oflag &= ~ONLCR;  // Disable new-line processing

  tty.c_cc[VTIME] = timeout_;  // Set timeout
  tty.c_cc[VMIN] = 0;          // Do not wait for data

  unsigned int baud_rate;
  switch (baud_rate_) {
    case Serial::BaudRate::BR9600:
      baud_rate = B9600;
      break;
    default:
      throw interface::SerialException(
          interface::SerialException::Type::SET_ATTRIBUTE, 0,
          "Invalid baud rate");
  }

  if (cfsetispeed(&tty, baud_rate) != 0) {  // Error setting input rate
    throw interface::SerialException(
        interface::SerialException::Type::SET_BAUD, errno,
        "Error setting input rate" + std::string(strerror(errno)));
  }
  if (cfsetospeed(&tty, baud_rate) != 0) {  // Error setting output rate
    throw interface::SerialException(
        interface::SerialException::Type::SET_BAUD, errno,
        "Error setting output rate" + std::string(strerror(errno)));
  }

  if (tcsetattr(fd_, TCSANOW, &tty) != 0) {  // Error setting attributes
    throw interface::SerialException(
        interface::SerialException::Type::SET_ATTRIBUTE, errno,
        "Error setting attributes" + std::string(strerror(errno)));
  }

  connected_ = true;
}

void interface::Serial::Disconnect() {
  if (!connected_) {
    throw interface::SerialException(
        interface::SerialException::Type::NOT_CONNECTED, 0,
        "Can not disconnect, not connected.");
  }

  if (close(fd_) != 0) {
    connected_ = false;
    throw interface::SerialException(
        interface::SerialException::Type::CLOSE, errno,
        "Error closing device" + std::string(strerror(errno)));
  }

  connected_ = false;
}

bool interface::Serial::IsConnected() { return connected_; }

std::string interface::Serial::Read() {
  if (!connected_) {
    throw interface::SerialException(
        interface::SerialException::Type::NOT_CONNECTED, 0,
        "Can not read, not connected.");
  }

  std::string data;
  char buffer[256];
  int bytes_read = 0;

  bytes_read = read(fd_, buffer, sizeof(buffer));

  if (bytes_read < 0) {
    throw interface::SerialException(
        interface::SerialException::Type::READ, errno,
        "Error reading from device" + std::string(strerror(errno)));
  }

  return std::string(buffer, bytes_read);
}

int interface::Serial::Write(std::string data) {
  if (!connected_) {
    throw interface::SerialException(
        interface::SerialException::Type::NOT_CONNECTED, 0,
        "Can not write, not connected.");
  }

  if (data.length() == 0) {
    return 0;
  }

  int bytes_written = write(fd_, data.c_str(), data.length());

  if (bytes_written < 0) {
    throw interface::SerialException(
        interface::SerialException::Type::WRITE, errno,
        "Error writing to device" + std::string(strerror(errno)));
  }

  return bytes_written;
}