/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Stream Test
 * @date 2023-05-15
 * @copyright Copyright (c) 2023
 */

#include <string>

#include "gtest/gtest.h"
#include "streams.h"

class Streams : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }
};

TEST_F(Streams, error_stream) {
  data::streams::ErrorStream es;
  data::streams::ErrorStreamPacket packet;

  packet.source = data::streams::Source::CONFIGURATION_MODULE;
  packet.code = "123";
  packet.info = "456";

  es.addPacket(packet);

  ASSERT_EQ(es.getNumPackets(), 1);
  ASSERT_EQ(es.getTotalPackets(), 1);

  data::streams::ErrorStreamPacket packet_from_stream;
  ASSERT_TRUE(es.getPacket(packet_from_stream));

  ASSERT_EQ(packet_from_stream.source, data::streams::Source::CONFIGURATION_MODULE);
  ASSERT_EQ(packet_from_stream.code, "123");
  ASSERT_EQ(packet_from_stream.info, "456");
}