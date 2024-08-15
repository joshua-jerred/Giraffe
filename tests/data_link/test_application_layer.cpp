#include "gtest/gtest.h"

#define GDL_APPLICATION_LAYER_SIMULATOR

#include <BoosterSeat/sleep.hpp>

#include <giraffe_data_link.hpp>
#include <giraffe_diagnostics.hpp>
#include <software_physical_layer.hpp>

using namespace giraffe;

gdl::Config config;

TEST(ApplicationLayer, EnableRequiresRadio) {

  gdl::DataLink gdl(config);
  EXPECT_FALSE(gdl.enable());
  EXPECT_FALSE(gdl.isEnabled());
  EXPECT_EQ(gdl.getStatus(), gdl::DataLink::Status::DISABLED);

  std::shared_ptr<gdl::SoftwarePhysicalLayer> spl =
      std::make_shared<gdl::SoftwarePhysicalLayer>(config);
  gdl.setPhysicalLayer(spl);

  EXPECT_TRUE(gdl.enable());
  bst::sleep(10);
  EXPECT_EQ(gdl.getStatus(), gdl::DataLink::Status::RUNNING);
  EXPECT_TRUE(gdl.isEnabled());
  EXPECT_TRUE(gdl.disable());
}

TEST(ApplicationLayer, ConstructWithPhysicalLayer) {

  std::shared_ptr<gdl::SoftwarePhysicalLayer> spl =
      std::make_shared<gdl::SoftwarePhysicalLayer>(config);

  gdl::DataLink gdl(config, spl);
  EXPECT_TRUE(gdl.enable());
  bst::sleep(10);
  EXPECT_EQ(gdl.getStatus(), gdl::DataLink::Status::RUNNING);
  EXPECT_TRUE(gdl.isEnabled());
}