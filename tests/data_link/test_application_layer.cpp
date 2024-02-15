#include "gtest/gtest.h"

#define GDL_APPLICATION_LAYER_SIMULATOR

#include <BoosterSeat/sleep.hpp>

#include <giraffe_data_link.hpp>
#include <giraffe_diagnostics.hpp>

// using namespace gdl;

// GdlConfig config;

TEST(ApplicationLayer, Start) {

  // GiraffeDataLink gdl(config);
  // EXPECT_EQ(gdl.getStatus(), GiraffeDataLink::Status::STOPPED);
  // gdl.start();
  // EXPECT_EQ(gdl.getStatus(), GiraffeDataLink::Status::STARTING);
  // try {
  // gdl.start();
  // FAIL();
  // } catch (GiraffeException &e) {
  // EXPECT_EQ(e.id(), DiagnosticId::GDL_invalidStartCall);
  // }
  // BoosterSeat::threadSleep(100);
  // EXPECT_EQ(gdl.getStatus(), GiraffeDataLink::Status::RUNNING);
}