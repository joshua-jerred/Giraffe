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
#include <BoosterSeat/string_formatting.hpp>
#include <BoosterSeat/timer.hpp>

#include "bit_types.hpp"
#include "i_built_in_test.hpp"
#include "logger.hpp"
#include "shared_data.hpp"
#include "test_cases.hpp"

namespace bit {

class TestCase {
public:
  // using TestFunction = std::function<TestStatus(data::SharedData &)>;

  TestCase(TestGroupId group_id, TestId test_id, data::SharedData &shared_data)
      : group_id_{group_id}, test_id_{test_id}, shared_data_{shared_data} {
  }
  ~TestCase() = default;

  TestStatus getStatus() const {
    return status_;
  }

  TestId getTestID() const {
    return test_id_;
  }

  std::string getTestIdString() const {
    constexpr bool WIDTH = 6;
    constexpr bool INCLUDE_0X = false;
    constexpr bool UPPERCASE = false;

    const uint16_t test_id_unsigned =
        bst::template_tools::to_underlying(test_id_);

    std::string id_unpaded = bst::string::intToHex(
        static_cast<int>(test_id_unsigned), WIDTH, INCLUDE_0X, UPPERCASE);
    std::string output = "0x";
    for (size_t i = 4; i > id_unpaded.size(); i--) {
      output += "0";
    }

    return output + id_unpaded;
  }

  void startTest() {
    status_ = TestStatus::WAITING;
  }

  void stopTest() {
    if (status_ == TestStatus::RUNNING) {
      status_ = TestStatus::STOPPED;
    }
  }

  void update() {
    if (status_ == TestStatus::WAITING) {
      status_ = TestStatus::RUNNING;
    }

    TestResult result;
    switch (test_id_) {
    case TestId::FCS_ErrorTest:
      result = test_case::runTestFCS_0001(shared_data_);
      break;
    case TestId::GPS_InstalledTest:
      result = test_case::runTestGPS_0400(shared_data_);
      break;
    default:
      result = {.test_id = test_id_,
                .group_id = group_id_,
                .status = TestStatus::UNKNOWN,
                .failure_reason = "Test case not found for Test ID."};
      break;
    }

    status_ = result.status;
    if (status_ == TestStatus::FAILED) {
      shared_data_.streams.log.error(node::Identification::FLIGHT_RUNNER,
                                     DiagnosticId::FLIGHT_RUNNER_bitTestFailed,
                                     "Built in test failed. Test ID: " +
                                         getTestIdString());
    }
  }

  void reset() {
    status_ = TestStatus::NOTRUN;
  }

private:
  TestGroupId group_id_;
  TestId test_id_;
  TestStatus status_ = TestStatus::NOTRUN;

  data::SharedData &shared_data_;
};

class TestGroup {
public:
  TestGroup(bit::TestGroupId group_id, data::SharedData &shared_data,
            std::vector<TestCase> test_cases)
      : group_id_{group_id}, shared_data_{shared_data},
        logger_{shared_data, node::Identification::FLIGHT_RUNNER, "BIT"},
        test_cases_{test_cases} {
  }

  ~TestGroup() = default;

  void start() {
    if (running_) {
      logger_.error(DiagnosticId::FLIGHT_RUNNER_bitTestStartFailure,
                    "Built in test is already running.");
      return;
    }

    setGroupStatus(TestStatus::WAITING);

    running_ = true;
    thread_ = std::thread(&TestGroup::groupRunner, this);
  }

  void stop() {
    running_ = false;
    if (thread_.joinable()) {
      thread_.join();
    }
    setGroupStatus(TestStatus::STOPPED);
  }

  void reset() {
    stop();
    setGroupStatus(TestStatus::NOTRUN);

    for (auto &test_case : test_cases_) {
      test_case.reset();
    }
  }

  TestStatus getGroupStatus() const {
    return group_status_;
  }

  Json getGroupResults() {
    std::lock_guard<std::mutex> lock(test_results_mutex_);

    Json results = Json::object();
    for (auto &test_case : test_cases_) {
      results[test_case.getTestIdString()] =
          testStatusToString(test_case.getStatus());
    }

    results["group_status"] = testStatusToString(group_status_);
    return results;
  }

private:
  void groupRunner() {
    setGroupStatus(TestStatus::RUNNING);

    for (auto &test_case : test_cases_) {
      test_case.startTest();
    }

    while (running_) {
      bst::sleep(50);

      const size_t MAX_TEST_TIME_MS = 1000;
      bst::Timer timer{MAX_TEST_TIME_MS};
      timer.reset();

      // Find the first incomplete test and run it.
      bool incomplete_test_found = false;
      for (auto &test_case : test_cases_) {

        while (!timer.isDone() &&
               (test_case.getStatus() == TestStatus::WAITING ||
                test_case.getStatus() == TestStatus::RUNNING)) {
          test_case.update();
          incomplete_test_found = true;
        }
      }

      // All tests are complete, break the loop.
      if (!incomplete_test_found) {
        running_ = false;
        break;
      }
    }

    // Run the cleanup for each test.
    for (auto &test_case : test_cases_) {
      test_case.stopTest();
    }

    // Check if all tests passed.
    bool all_tests_passed = true;
    for (auto &test_case : test_cases_) {
      if (test_case.getStatus() != TestStatus::PASSED) {
        all_tests_passed = false;
        break;
      }
    }

    setGroupStatus(all_tests_passed ? TestStatus::PASSED : TestStatus::FAILED);
  }

  void setGroupStatus(TestStatus status) {
    group_status_ = status;
  }

  bit::TestGroupId group_id_;
  data::SharedData &shared_data_;
  giraffe::Logger logger_;
  std::vector<TestCase> test_cases_;

  std::thread thread_{};
  std::atomic<bool> running_{false};

  std::atomic<TestStatus> group_status_{TestStatus::NOTRUN};
  std::mutex test_results_mutex_{};
};

class BuiltInTest : public IBuiltInTest {
public:
  BuiltInTest(data::SharedData &shared_data)
      : shared_data_{shared_data}, logger_{shared_data,
                                           node::Identification::FLIGHT_RUNNER,
                                           "BIT"} {
    updateBitTestData();
  }

  ~BuiltInTest() {
    if (running_) {
      stop();
    }

    if (thread_.joinable()) {
      thread_.join();
    }

    for (auto &test_group : test_groups_map_) {
      test_group.second.stop();
    }
  }

  void start() {
    if (running_) {
      logger_.error(DiagnosticId::FLIGHT_RUNNER_bitTestStartFailure,
                    "Built in test is already running.");
      return;
    }

    if (thread_.joinable()) {
      logger_.error(DiagnosticId::FLIGHT_RUNNER_bitTestStartFailure,
                    "Thread is joinable.");
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

  /// @brief Reset the BIT system to it's initial state.
  /// @warning Risk of race conditions here. It's just BIT, so low risk.
  void reset() {
    if (running_) {
      logger_.error(DiagnosticId::FLIGHT_RUNNER_bitTestResetFailure,
                    "BIT running, cannot reset. Stop first.");
      return;
    }

    if (thread_.joinable()) {
      thread_.join();
    }

    general_bit_status_ = TestStatus::NOTRUN;
    for (auto &test_group : test_groups_map_) {
      test_group.second.reset();
    }

    updateBitTestData();
  }

  void getBitTestData(Json &out_data) override {
    updateBitTestData();

    std::lock_guard<std::mutex> lock(test_data_mutex_);
    out_data = test_data_buffer_;
  }

private:
  void updateBitTestData() {
    Json bit_info = {
        {"gfs_ready_to_perform_bit_test",
         static_cast<bool>(shared_data_.flight_data.getFlightPhase() ==
                           FlightPhase::PRE_LAUNCH)},
        {"bit_test_running", running_.load()},
        {"bit_test_start_time", {}},
        {"bit_test_end_time", {}},
        {"bit_test_results", testStatusToString(general_bit_status_)},
    };

    Json bit_results = Json::object();
    for (auto &test_group : test_groups_map_) {
      bit_results[testGroupIdToString(test_group.first)] =
          test_group.second.getGroupResults();
    }

    Json results =
        Json::object({{"bit_info", bit_info}, {"bit_results", bit_results}});

    shared_data_.flight_data.setBitTestData(results);

    std::lock_guard<std::mutex> lock(test_data_mutex_);
    test_data_buffer_ = results;
  }

  void bitTestRunner() {
    general_bit_status_ = TestStatus::RUNNING;

    // Startup all of the test groups.
    for (auto &test_group : test_groups_map_) {
      test_group.second.start();
    }

    while (running_) {
      updateBitTestData();

      bool all_tests_complete = true;
      for (auto &test_group : test_groups_map_) {
        if (test_group.second.getGroupStatus() == TestStatus::WAITING ||
            test_group.second.getGroupStatus() == TestStatus::RUNNING) {
          all_tests_complete = false;
        }
      }

      if (all_tests_complete) {
        running_ = false;
        break;
      }

      bst::sleep(500);
    }

    bool all_tests_passed = true;
    for (auto &test_group : test_groups_map_) {
      if (test_group.second.getGroupStatus() != TestStatus::PASSED) {
        all_tests_passed = false;
        break;
      }
    }

    general_bit_status_ =
        all_tests_passed ? TestStatus::PASSED : TestStatus::FAILED;

    updateBitTestData();
  }

  std::mutex test_data_mutex_{};
  Json test_data_buffer_{};

  data::SharedData &shared_data_;
  giraffe::Logger logger_;

  bit::TestStatus general_bit_status_{bit::TestStatus::NOTRUN};

  std::thread thread_{};
  std::atomic<bool> running_{false};

  //<{{bit_test_groups}}@

  /// @brief Test cases for the FCS group.
   TestGroup fcs_test_group_{
    TestGroupId::FCS, shared_data_,
    {{TestGroupId::FCS, TestId::FCS_ErrorTest, shared_data_},
    {TestGroupId::FCS, TestId::FCS_FlightPhasePredictorTest, shared_data_},
    {TestGroupId::FCS, TestId::FCS_LaunchPositionTest, shared_data_},
    {TestGroupId::FCS, TestId::FCS_DataModuleProcessingTest, shared_data_},
    },
  };

  /// @brief Test cases for the SYS group.
   TestGroup sys_test_group_{
    TestGroupId::SYS, shared_data_,
    {{TestGroupId::SYS, TestId::SYS_ErrorTest, shared_data_},
    },
  };

  /// @brief Test cases for the DLNK group.
   TestGroup dlnk_test_group_{
    TestGroupId::DLNK, shared_data_,
    {{TestGroupId::DLNK, TestId::DLNK_ConfigLocationDataTest, shared_data_},
    },
  };

  /// @brief Test cases for the GPS group.
   TestGroup gps_test_group_{
    TestGroupId::GPS, shared_data_,
    {{TestGroupId::GPS, TestId::GPS_InstalledTest, shared_data_},
    },
  };

  /// @brief Test cases for the BATT group.
   TestGroup batt_test_group_{
    TestGroupId::BATT, shared_data_,
    {{TestGroupId::BATT, TestId::BATT_AdcTest, shared_data_},
    },
  };

  /// @brief Test cases for the EXTN group.
   TestGroup extn_test_group_{
    TestGroupId::EXTN, shared_data_,
    {{TestGroupId::EXTN, TestId::EXTN_ExtensionsRunningTest, shared_data_},
    },
  };

  /// @brief Test cases for the ENV group.
   TestGroup env_test_group_{
    TestGroupId::ENV, shared_data_,
    {{TestGroupId::ENV, TestId::ENV_TemperatureTest, shared_data_},
    },
  };

  /// @brief Test cases for the CAM group.
   TestGroup cam_test_group_{
    TestGroupId::CAM, shared_data_,
    {{TestGroupId::CAM, TestId::CAM_InstalledTest, shared_data_},
    },
  };

  /// @brief Test cases for the IMU group.
   TestGroup imu_test_group_{
    TestGroupId::IMU, shared_data_,
    {{TestGroupId::IMU, TestId::IMU_InstalledTest, shared_data_},
    },
  };

  /// @brief Test cases for the ADC group.
   TestGroup adc_test_group_{
    TestGroupId::ADC, shared_data_,
    {{TestGroupId::ADC, TestId::ADC_InstalledTest, shared_data_},
    },
  };

  /// @brief This map contains the collection of test groups.
  std::map<TestGroupId, TestGroup &> test_groups_map_{
    {TestGroupId::FCS, fcs_test_group_},
    {TestGroupId::SYS, sys_test_group_},
    {TestGroupId::DLNK, dlnk_test_group_},
    {TestGroupId::GPS, gps_test_group_},
    {TestGroupId::BATT, batt_test_group_},
    {TestGroupId::EXTN, extn_test_group_},
    {TestGroupId::ENV, env_test_group_},
    {TestGroupId::CAM, cam_test_group_},
    {TestGroupId::IMU, imu_test_group_},
    {TestGroupId::ADC, adc_test_group_},
  };

  //@{{bit_test_groups}}>
};

} // namespace bit