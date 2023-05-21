/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Stream Test
 * @date 2023-05-15
 * @copyright Copyright (c) 2023
 */

#include <iostream>
#include <string>
#include <thread>

#include "gtest/gtest.h"
#include "module.h"
#include "streams.h"

data::Streams streams;

modules::MetaData metadata("test_module", data::Source::NONE,
                           command::Destination::UNKNOWN);

class BasicModule : public modules::Module {
 public:
  BasicModule(data::Streams &streams) : modules::Module(metadata, streams) {
  }

 private:
  void startup() override {
    data<std::string>("ident", "start");
  }
  void loop() override {
    data<std::string>("ident", "loop");
  }
  void shutdown() override {
    data<std::string>("ident", "stop");
    std::cout << "shutdown" << std::endl;
  }
  void processCommand(const command::Command &command) override {
    (void)command;
  }
};

class ModuleTests : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }
};

TEST_F(ModuleTests, start_and_stop) {
  BasicModule mod(streams);
  mod.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  mod.stop();
}