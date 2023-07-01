#include "gtest/gtest.h"

#include "gdl_configuration.hpp"
#include "gdl_message.hpp"
#include "gdl_message_queue.hpp"

TEST(GdlMessageQueueTest, Push) {
  constexpr uint8_t max_queue_size = 10;
  gdl::MessageQueue queue{max_queue_size};

  gdl::Message message{1, gdl::Message::Type::UNDEFINED, "test"};

  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.push(message));
  EXPECT_EQ(queue.size(), 1);
}

TEST(GdlMessageQueueTest, PushFull) {
  constexpr uint8_t max_queue_size = 10;
  gdl::MessageQueue queue{max_queue_size};

  gdl::Message message{1, gdl::Message::Type::UNDEFINED, "test"};

  for (uint8_t i = 0; i < max_queue_size; ++i) {
    EXPECT_TRUE(queue.push(message));
  }

  EXPECT_FALSE(queue.push(message));
}

TEST(GdlMessageQueueTest, Pop) {
  constexpr uint8_t max_queue_size = 10;
  gdl::MessageQueue queue{max_queue_size};

  gdl::Message message{1, gdl::Message::Type::UNDEFINED, "test"};

  for (uint8_t i = 0; i < max_queue_size; ++i) {
    EXPECT_TRUE(queue.push(message));
  }

  EXPECT_EQ(queue.size(), max_queue_size);

  for (uint8_t i = 0; i < max_queue_size; ++i) {
    gdl::Message popped_message;
    EXPECT_TRUE(queue.pop(popped_message));
    EXPECT_EQ(popped_message.data, message.data);
    EXPECT_EQ(popped_message.id, message.id);
  }

  EXPECT_EQ(queue.size(), 0);
}

TEST(GdlMessageQueueTest, PopEmpty) {
  constexpr uint8_t max_queue_size = 10;
  gdl::MessageQueue queue{max_queue_size};

  gdl::Message popped_message;
  EXPECT_FALSE(queue.pop(popped_message));
}