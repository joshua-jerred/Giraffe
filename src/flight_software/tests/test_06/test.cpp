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
  data::ErrorStream es;
  data::ErrorStreamPacket packet;

  packet.source = data::Source::CONFIGURATION;
  packet.code = "123";
  packet.info = "456";

  es.addPacket(packet);

  EXPECT_EQ(es.getNumPackets(), 1);
  EXPECT_EQ(es.getTotalPackets(), 1);

  data::ErrorStreamPacket packet_from_stream;
  ASSERT_TRUE(es.getPacket(packet_from_stream));

  EXPECT_EQ(es.getNumPackets(), 0);
  EXPECT_EQ(es.getTotalPackets(), 1);

  EXPECT_EQ(packet_from_stream.source, data::Source::CONFIGURATION);
  EXPECT_EQ(packet_from_stream.code, "123");
  EXPECT_EQ(packet_from_stream.info, "456");
}