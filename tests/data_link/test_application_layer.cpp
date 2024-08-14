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
  EXPECT_EQ(gdl.getStatus(), gdl::DataLink::Status::DISCONNECTED);
  EXPECT_TRUE(gdl.isEnabled());

  // BoosterSeat::threadSleep(100);
  // EXPECT_EQ(gdl.getStatus(), GiraffeDataLink::Status::RUNNING);
}