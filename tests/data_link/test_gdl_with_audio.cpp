#include "gtest/gtest.h"

#include <BoosterSeat/filesystem.hpp>
#include <BoosterSeat/sleep.hpp>

#include <giraffe_data_link.hpp>
#include <software_physical_layer.hpp>

using namespace giraffe;

TEST(gdl_with_audio, broadcast_text) {
  GTEST_SKIP();
  gdl::Config config;
  auto sw_pysical_layer = createSwPhysicalLayer(config);
  gdl::DataLink datalink(config, sw_pysical_layer);
  EXPECT_TRUE(datalink.enable());

  gdl::Message message;
  message.setBroadcastMessage("Hello, World!", 0);
  EXPECT_TRUE(datalink.sendMessage(message));
  bst::sleep(500);
}

TEST(gdl_with_audio, broadcast_image) {
  GTEST_SKIP();
  gdl::Config config;
  auto sw_pysical_layer = createSwPhysicalLayer(config);
  gdl::DataLink datalink(config, sw_pysical_layer);
  EXPECT_TRUE(datalink.enable());

  const std::string TEST_IMAGE_PATH = "sstv_image.png";
  EXPECT_TRUE(bst::filesystem::doesFileExist(TEST_IMAGE_PATH));

  gdl::Message::Image image;
  image.path = TEST_IMAGE_PATH;

  gdl::Message message;
  message.setImageMessage(image, 0);
  EXPECT_TRUE(datalink.sendMessage(message));
  bst::sleep(500);
}