/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Basic test for the GFS protocol C++ implementation.
 * @date 2023-05-15
 * @copyright Copyright (c) 2023
 */

#include <nlohmann/json.hpp>
#include <iostream>

#include "gtest/gtest.h"
#include "protocol.h"

using json = nlohmann::ordered_json;
using namespace protocol;

class GfsProtocol : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }
};

TEST_F(GfsProtocol, ToString) { 
  Message basic_message(
    Endpoint::GFS,
    Endpoint::GGS,
    Type::REQUEST,
    Category::PING,
    "this_is_the_id",
    "{body}"
  );
  std::cout << basic_message.getMessageString();
  json message_string = json::parse(basic_message.getMessageString());
}