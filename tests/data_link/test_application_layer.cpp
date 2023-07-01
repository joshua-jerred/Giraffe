#include "gtest/gtest.h"

#define GDL_APPLICATION_LAYER_SIMULATOR
#include <giraffe_data_link.hpp>

using namespace gdl;

GdlConfig config;

TEST(ApplicationLayer, StateMachine) {
  GiraffeDataLink gdl(config);
  EXPECT_EQ(gdl.getStatus(), GiraffeDataLink::Status::STOPPED);
  gdl.start();
  EXPECT_EQ(gdl.getStatus(), GiraffeDataLink::Status::STARTING);
}