#include "gtest/gtest.h"

#include "gdl_configuration.hpp"
#include "gdl_message.hpp"
#include "message_queue.hpp"

TEST(GdlMessageQueueTest, Push) {
  constexpr uint8_t max_queue_size = 10;
  gdl::MessageQueue queue{max_queue_size};

  gdl::Message message{1, "test"};

  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.push(message));
  EXPECT_EQ(queue.size(), 1);
}