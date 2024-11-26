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
#include <vector>

#include <BoosterSeat/sleep.hpp>
#include <BoosterSeat/stopwatch.hpp>

#include "bit_test_enums.hpp"
#include "i_built_in_test.hpp"
#include "logger.hpp"
#include "shared_data.hpp"

namespace bit {

class TestCase {
public:
  // using TestFunction = std::function<TestStatus(data::SharedData &)>;

  TestCase(TestGroupId group_id, TestId test_id)
      : group_id_{group_id}, test_id_{test_id} {
  }

private:
  TestGroupId group_id_;
  TestId test_id_;
  TestStatus status_ = TestStatus::NotRun;
};

class TestSuite {
public:
  TestSuite(bit::TestGroupId group_id, data::SharedData &shared_data,
            std::vector<TestCase> test_cases)
      : group_id_{group_id}, shared_data_{shared_data},
        logger_{shared_data, node::Identification::FLIGHT_RUNNER, "BIT"},
        test_cases_{test_cases} {
  }

  ~TestSuite() = default;

  // void start() {
  //   if (running_) {
  //     logger_.error(DiagnosticId::FLIGHT_RUNNER_bitTestStartFailure,
  //                   "Built in test is already running.");
  //     return;
  //   }

  //   running_ = true;
  //   thread_ = std::thread(&TestSuite::bitTestRunner, this);
  // }

private:
  bit::TestGroupId group_id_;
  data::SharedData &shared_data_;
  giraffe::Logger logger_;
  std::vector<TestCase> test_cases_;
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

  void stop() {
    if (!running_) {
      logger_.error(DiagnosticId::FLIGHT_RUNNER_bitTestStopResetFailure,
                    "Built in test is not running.");
      return;
    }

    running_ = false;
    thread_.join();
  }

  void getBitTestData(Json &out_data) override {
    std::lock_guard<std::mutex> lock(test_data_mutex_);
    out_data = test_data_buffer_;
  }

private:
  void bitTestRunner() {
    while (running_) {
      bst::sleep(50);
      running_ = false;
    }
  }

  std::mutex test_data_mutex_{};
  Json test_data_buffer_{};

  data::SharedData &shared_data_;
  giraffe::Logger logger_;

  bit::TestStatus general_bit_status_{bit::TestStatus::NotRun};

  std::thread thread_{};
  std::atomic<bool> running_{false};

  //<{{bit_test_suites}}@

  /// @brief Test cases for the FCS group.
  TestSuite fcs_test_suite_{
      TestGroupId::FCS,
      shared_data_,
      {
          {TestGroupId::FCS, TestId::FCS_ErrorTest},
          {TestGroupId::FCS, TestId::FCS_FlightPhasePredictorTest},
          {TestGroupId::FCS, TestId::FCS_LaunchPositionTest},
          {TestGroupId::FCS, TestId::FCS_DataModuleProcessingTest},
      },
  };

  /// @brief Test cases for the SYS group.
  TestSuite sys_test_suite_{
      TestGroupId::SYS,
      shared_data_,
      {
          {TestGroupId::SYS, TestId::SYS_ErrorTest},
      },
  };

  /// @brief Test cases for the DLNK group.
  TestSuite dlnk_test_suite_{
      TestGroupId::DLNK,
      shared_data_,
      {
          {TestGroupId::DLNK, TestId::DLNK_ConfigLocationDataTest},
      },
  };

  /// @brief Test cases for the GPS group.
  TestSuite gps_test_suite_{
      TestGroupId::GPS,
      shared_data_,
      {
          {TestGroupId::GPS, TestId::GPS_InstalledTest},
      },
  };

  /// @brief Test cases for the BATT group.
  TestSuite batt_test_suite_{
      TestGroupId::BATT,
      shared_data_,
      {
          {TestGroupId::BATT, TestId::BATT_AdcTest},
      },
  };

  /// @brief Test cases for the EXTN group.
  TestSuite extn_test_suite_{
      TestGroupId::EXTN,
      shared_data_,
      {
          {TestGroupId::EXTN, TestId::EXTN_ExtensionsRunningTest},
      },
  };

  /// @brief Test cases for the ENV group.
  TestSuite env_test_suite_{
      TestGroupId::ENV,
      shared_data_,
      {
          {TestGroupId::ENV, TestId::ENV_TemperatureTest},
      },
  };

  /// @brief Test cases for the CAM group.
  TestSuite cam_test_suite_{
      TestGroupId::CAM,
      shared_data_,
      {
          {TestGroupId::CAM, TestId::CAM_InstalledTest},
      },
  };

  /// @brief Test cases for the IMU group.
  TestSuite imu_test_suite_{
      TestGroupId::IMU,
      shared_data_,
      {
          {TestGroupId::IMU, TestId::IMU_InstalledTest},
      },
  };

  /// @brief Test cases for the ADC group.
  TestSuite adc_test_suite_{
      TestGroupId::ADC,
      shared_data_,
      {
          {TestGroupId::ADC, TestId::ADC_InstalledTest},
      },
  };

  std::map<TestGroupId, TestSuite &> test_groups_map{
      {TestGroupId::FCS, fcs_test_suite_},
      {TestGroupId::SYS, sys_test_suite_},
      {TestGroupId::DLNK, dlnk_test_suite_},
      {TestGroupId::GPS, gps_test_suite_},
      {TestGroupId::BATT, batt_test_suite_},
      {TestGroupId::EXTN, extn_test_suite_},
      {TestGroupId::ENV, env_test_suite_},
      {TestGroupId::CAM, cam_test_suite_},
      {TestGroupId::IMU, imu_test_suite_},
      {TestGroupId::ADC, adc_test_suite_},
  };

  //@{{bit_test_suites}}>
};

} // namespace bit