/**
 *
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 *
 *
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 * 
 * 
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-11-16
 * @copyright  2023 (license to be defined)
 * =*=======================*=
 *
 * @verbatim
 */

#include <cstdint>
#include <string>

namespace bit {

  /// @brief This enum contains the status codes for the Built-In Test (BIT) system.
  enum class TestStatus : uint8_t {
    /// @brief UNK
    Unknown = 0,
    /// @brief N/D
    NoData = 1,
    /// @brief N/R
    NotRun = 2,
    /// @brief WAIT
    Waiting = 3,
    /// @brief RUN
    Running = 4,
    /// @brief PASS
    Passed = 5,
    /// @brief FAIL
    Failed = 6,
    /// @brief SKIP
    Skipped = 7,
  };

  extern std::string testStatusToString(TestStatus status);

  /// @brief Identifiers for the different groups of BIT tests.
  enum class BitTestGroup : uint8_t {
    /// @brief Flight Control System
    FlightControlSystem,
    /// @brief System
    System,
    /// @brief Data Link
    DataLink,
    /// @brief GPS
    GPS,
    /// @brief Battery
    Battery,
    /// @brief Extensions
    Extensions,
    /// @brief Environment
    Environment,
    /// @brief Camera
    Camera,
    /// @brief Inertial Measurement Unit
    InertialMeasurementUnit,
    /// @brief Analog to Digital Converter
    AnalogtoDigitalConverter,
  };

  /// @brief This enum contains the test ids for the Built-In Test (BIT) system.
  enum class BitTestId : uint16_t {
    FCS_ErrorTest = 0x0000,
    SYS_ErrorTest = 0x0100,
    DLNK_ConfigLocationDataTest = 0x0200,
    GPS_InstalledTest = 0x0400,
    BATT_AdcTest = 0x0500,
    EXTN_ExtensionsRunningTest = 0x0600,
    ENV_TemperatureTest = 0x0700,
    CAM_InstalledTest = 0x0800,
    IMU_InstalledTest = 0x0900,
    ADC_InstalledTest = 0x0a00,
  };

}

/**
 *
 * Generated on: 2024-11-16
 * 
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */