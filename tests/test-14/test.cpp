/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Test-14 - Serial Interface
 * @date 2023-03-17
 * @copyright Copyright (c) 2023
 * @version 0.4
 */

#include <chrono>
#include <string>
#include <thread>

#include "interface.h"
#include "gtest/gtest.h"

class SerialInterface : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(SerialInterface, SerialInterface) {
  try {
    interface::Serial serial("/dev/ttyS0", interface::Serial::BaudRate::BR9600);
    serial.Connect();
    serial.Write("AT+DMOCONNECT\r\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::string response = serial.Read();
    EXPECT_EQ(response, "+DMOCONNECT:0\r\n") << "Serial Handshake Failed";
  } catch (interface::SerialException& e) {
    FAIL() << e.what();
  }
}