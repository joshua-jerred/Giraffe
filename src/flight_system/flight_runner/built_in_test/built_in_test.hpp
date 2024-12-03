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

#include "bit_types.hpp"
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
  ~TestCase() = default;

  TestStatus getStatus() const {
    return status_;
  }

  TestId getTestID() const {
    return test_id_;
  }

  void startTest() {
    status_ = TestStatus::WAITING;
  }

  void stopTest() {
    if (status_ == TestStatus::WAITING || status_ == TestStatus::RUNNING) {
      status_ = TestStatus::STOPPED;
    }
  }

  void update() {
    if (status_ == TestStatus::WAITING) {
      status_ = TestStatus::RUNNING;
    }
  }

private:
  TestGroupId group_id_;
  TestId test_id_;
  TestStatus status_ = TestStatus::NOTRUN;
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
  }

  TestStatus getGroupStatus() const {
    return group_status_;
  }

  Json getGroupResults() {
    std::lock_guard<std::mutex> lock(test_results_mutex_);

    Json results = Json::object();
    for (auto &test_case : test_cases_) {
      results[testGroupIdToString(group_id_)] = {
          testIdToString(test_case.getTestID()),
          testStatusToString(test_case.getStatus())};
    }
    return results;
  }

private:
  void groupRunner() {
    while (running_) {
      bst::sleep(100);

      for (auto &test_case : test_cases_) {
        test_case.startTest();

        bst::sleep(5000);
        /// @todo tmp, run the tests here.
        test_case.update();
        bst::sleep(5000);

        test_case.stopTest();
      }
    }
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

    while (running_) {
      updateBitTestData();

      bst::sleep(500);
    }
    general_bit_status_ = TestStatus::STOPPED;
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
  TestGroup sys_test_group_{
      TestGroupId::SYS,
      shared_data_,
      {
          {TestGroupId::SYS, TestId::SYS_ErrorTest},
      },
  };

  /// @brief Test cases for the DLNK group.
  TestGroup dlnk_test_group_{
      TestGroupId::DLNK,
      shared_data_,
      {
          {TestGroupId::DLNK, TestId::DLNK_ConfigLocationDataTest},
      },
  };

  /// @brief Test cases for the GPS group.
  TestGroup gps_test_group_{
      TestGroupId::GPS,
      shared_data_,
      {
          {TestGroupId::GPS, TestId::GPS_InstalledTest},
      },
  };

  /// @brief Test cases for the BATT group.
  TestGroup batt_test_group_{
      TestGroupId::BATT,
      shared_data_,
      {
          {TestGroupId::BATT, TestId::BATT_AdcTest},
      },
  };

  /// @brief Test cases for the EXTN group.
  TestGroup extn_test_group_{
      TestGroupId::EXTN,
      shared_data_,
      {
          {TestGroupId::EXTN, TestId::EXTN_ExtensionsRunningTest},
      },
  };

  /// @brief Test cases for the ENV group.
  TestGroup env_test_group_{
      TestGroupId::ENV,
      shared_data_,
      {
          {TestGroupId::ENV, TestId::ENV_TemperatureTest},
      },
  };

  /// @brief Test cases for the CAM group.
  TestGroup cam_test_group_{
      TestGroupId::CAM,
      shared_data_,
      {
          {TestGroupId::CAM, TestId::CAM_InstalledTest},
      },
  };

  /// @brief Test cases for the IMU group.
  TestGroup imu_test_group_{
      TestGroupId::IMU,
      shared_data_,
      {
          {TestGroupId::IMU, TestId::IMU_InstalledTest},
      },
  };

  /// @brief Test cases for the ADC group.
  TestGroup adc_test_group_{
      TestGroupId::ADC,
      shared_data_,
      {
          {TestGroupId::ADC, TestId::ADC_InstalledTest},
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