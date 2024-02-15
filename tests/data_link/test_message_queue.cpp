#include "gtest/gtest.h"

#include "gdl_constants.hpp"
#include "gdl_message.hpp"

using namespace giraffe;

TEST(GdlMessageQueueTest, Push) {
  gdl::MessageQueue queue{};

  gdl::Message message;
  message.setBroadcastMessage("test", 1);

  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.push(message));
  EXPECT_EQ(queue.size(), 1);
}

TEST(GdlMessageQueueTest, PushFull) {
  // GTEST_SKIP();
  gdl::MessageQueue queue{};

  gdl::Message message;
  message.setBroadcastMessage("test", 1);

  for (uint8_t i = 0; i < gdl::GDL_MESSAGE_QUEUES_SIZE; ++i) {
    EXPECT_TRUE(queue.push(message));
  }

  EXPECT_FALSE(queue.push(message));
}

TEST(GdlMessageQueueTest, Pop) {
  gdl::MessageQueue queue{};

  gdl::Message message;
  message.setBroadcastMessage("test", 1);

  for (uint8_t i = 0; i < gdl::GDL_MESSAGE_QUEUES_SIZE; ++i) {
    EXPECT_TRUE(queue.push(message));
  }

  EXPECT_EQ(queue.size(), gdl::GDL_MESSAGE_QUEUES_SIZE);

  for (uint8_t i = 0; i < gdl::GDL_MESSAGE_QUEUES_SIZE; ++i) {
    gdl::Message popped_message;
    EXPECT_TRUE(queue.pop(popped_message));
    EXPECT_EQ(popped_message.getData(), message.getData());
    EXPECT_EQ(popped_message.getIdentifier(), message.getIdentifier());
  }

  EXPECT_EQ(queue.size(), 0);
}

TEST(GdlMessageQueueTest, PopEmpty) {
  constexpr uint8_t max_queue_size = 10;
  gdl::MessageQueue queue{};

  gdl::Message popped_message;
  EXPECT_FALSE(queue.pop(popped_message));
}