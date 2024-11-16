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

#include <BoosterSeat/sleep.hpp>
#include <BoosterSeat/stopwatch.hpp>

#include "built_in_test_enums.hpp"
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
    if (running_) {
      logger_.error(DiagnosticId::FLIGHT_RUNNER_bitTestStartFailure,
                    "Built in test is already running.");
      return;
    }

    running_ = true;
    thread_ = std::thread(&BuiltInTest::bitTestRunner, this);
  }

private:
  void bitTestRunner() {
    while (running_) {
      // Run test
      bst::sleep(50);
    }
  }

  data::SharedData &shared_data_;
  giraffe::Logger logger_;

  std::thread thread_{};
  std::atomic<bool> running_{false};
};