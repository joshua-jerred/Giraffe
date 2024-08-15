#include "gtest/gtest.h"

#include "gdl_message.hpp"

using namespace giraffe;

TEST(GdlMessage, BroadcastMessage) {
  gdl::Message message;
  message.setBroadcastMessage("test1", 1);

  EXPECT_EQ(message.getIdentifier(), 1);
  EXPECT_EQ(message.getType(), gdl::Message::Type::BROADCAST);
  EXPECT_EQ(message.getData(), "test1");
  EXPECT_TRUE(message.verifyType());
}

TEST(GdlMessage, ExchangeMessage) {
  gdl::Message message;
  message.setExchangeMessage("test2", 2);

  EXPECT_EQ(message.getIdentifier(), 2);
  EXPECT_EQ(message.getType(), gdl::Message::Type::EXCHANGE);
  EXPECT_EQ(message.getData(), "test2");
  EXPECT_TRUE(message.verifyType());
}

TEST(GdlMessage, LocationMessage) {
  gdl::Message::Location location{.latitude = 3,
                                  .longitude = 4,
                                  .altitude = 1,
                                  .speed = 5,
                                  .heading = 2,
                                  .time_code = "test"};

  gdl::Message message;
  message.setLocationMessage(location, 3);

  EXPECT_EQ(message.getIdentifier(), 3);
  EXPECT_EQ(message.getType(), gdl::Message::Type::LOCATION);
  EXPECT_TRUE(message.verifyType());

  const auto loc = message.getLocation();
  EXPECT_EQ(loc.latitude, location.latitude);
  EXPECT_EQ(loc.longitude, location.longitude);
  EXPECT_EQ(loc.altitude, location.altitude);
  EXPECT_EQ(loc.speed, location.speed);
  EXPECT_EQ(loc.heading, location.heading);
  EXPECT_EQ(loc.time_code, location.time_code);
}

TEST(GdlMessage, ImageMessage) {
  gdl::Message::Image image{.path = "test"};

  gdl::Message message;
  message.setImageMessage(image, 4);

  EXPECT_EQ(message.getIdentifier(), 4);
  EXPECT_EQ(message.getType(), gdl::Message::Type::IMAGE);
  EXPECT_TRUE(message.verifyType());

  const auto img = message.getImage();
  EXPECT_EQ(img.path, image.path);
}

TEST(GdlMessage, VerifyType) {
  gdl::Message message;
  message.setExchangeMessage("test2", 5);

  EXPECT_EQ(message.getIdentifier(), 5);
  EXPECT_EQ(message.getType(), gdl::Message::Type::EXCHANGE);
  EXPECT_EQ(message.getData(), "test2");
  EXPECT_TRUE(message.verifyType());

  message.setType(gdl::Message::Type::BROADCAST);
  EXPECT_TRUE(message.verifyType());

  message.setType(gdl::Message::Type::LOCATION);
  EXPECT_FALSE(message.verifyType());
}