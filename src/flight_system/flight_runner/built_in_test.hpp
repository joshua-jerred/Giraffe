/// =*========GIRAFFE========*=
/// A Unified Flight Command and Control System
/// https://github.com/joshua-jerred/Giraffe
/// https://giraffe.joshuajer.red/
/// =*=======================*=
///
/// @file   built_in_test.hpp
///
/// =*=======================*=
/// @author     Joshua Jerred (https://joshuajer.red)
/// @date       2024-11-16
/// @copyright  2024 (license to be defined)

#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>

#include "logger.hpp"
#include "shared_data.hpp"

class IBuiltInTest {
public:
  virtual ~IBuiltInTest() = default;
};

class BuiltInTest : public IBuiltInTest {
public:
  BuiltInTest(data::SharedData &shared_data)
      : shared_data_{shared_data}, logger_{shared_data,
                                           node::Identification::FLIGHT_RUNNER,
                                           "BIT"} {
  }
  ~BuiltInTest() = default;

  void start() {
    std::thread test_thread(&BuiltInTest::run_test, this);
    test_thread.join();
  }

private:
  void run_test() {
    while (true) {
      // Run test
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

  data::SharedData &shared_data_;
  Logger logger_;
};