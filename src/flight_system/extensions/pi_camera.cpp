/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   pi_camera.cpp
 * @brief  Raspberry Pi Camera Extension
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-13
 * @copyright  2024 (license to be defined)
 */

#include "pi_camera.hpp"

#include "flight_computer_info.hpp"
#include "giraffe_exception.hpp"
#include "giraffe_file_paths.hpp"

#include <BoosterSeat/process.hpp>
#include <BoosterSeat/string_utils.hpp>
#include <BoosterSeat/time.hpp>

namespace extension {

PiCamera::PiCamera(ExtensionResources &resources,
                   cfg::ExtensionMetadata metadata)
    : Extension(resources, metadata) {
  try {
    image_dir_ = giraffe::file_paths::getGfsImageDirPath();
  } catch (const GiraffeException &e) {
    error(e.id());
    image_dir_ = "";
  }
}

void PiCamera::startup() {
  if (image_dir_.empty()) {
    raiseFault(DiagnosticId::EXT_FAULT_piCameraInvalidImageDir);
    return;
  }

  // detect if the fc is a raspberry pi
  auto fc_info = giraffe::getFlightComputerInfo();
  if (!fc_info.isRaspberryPi()) {
    raiseFault(DiagnosticId::EXT_FAULT_piCameraNotOnPi);
    return;
  }

  // detect if the camera is connected
  // removed as rpicam-detect must be built and installed
  // try {
  //   bst::Process detect_proc("rpicam-detect", {});
  //   detect_proc.waitToComplete();

  //   if (detect_proc.getExitCode() != 0) {
  //     raiseFault(DiagnosticId::EXT_FAULT_piCameraDetectFailed, "ec");
  //     return;
  //   }

  //   const std::string &detect_output = detect_proc.getStdout();

  //   /// @todo parse the output to determine if the camera is connected

  // } catch (const bst::BoosterSeatException &e) {
  //   raiseFault(DiagnosticId::EXT_FAULT_piCameraDetectFailed, "except");
  //   return;
  // }
}

void PiCamera::loop() {
  constexpr unsigned int K_CAPTURE_TIMEOUT_MS = 4000;
  constexpr uint32_t K_MAX_CAPTURE_FAILURES = 3;

  std::string image_file_name = std::move(getImageName());
  std::string image_file_path = image_dir_ + "/" + image_file_name;

  if (capture_failures_ >= K_MAX_CAPTURE_FAILURES) {
    raiseFault(DiagnosticId::EXT_FAULT_piCameraCaptureFailed);
  }

  try {
    bst::Process capture_proc(
        "rpicam-still", {"-e", "jpg", "--immediate", "-o", image_file_path});
    capture_proc.waitToComplete(K_CAPTURE_TIMEOUT_MS);

    if (capture_proc.getExitCode() != 0) {
      capture_failures_++;
      error(DiagnosticId::EXTENSION_piCameraCaptureFail,
            "exit cd: " + std::to_string(capture_proc.getExitCode()));
      debug("Capture error: " + capture_proc.getStderr());
      return;
    }

    /// @note rpicam-still does not output anything to stdout, only stderr
    const std::string &capture_error = capture_proc.getStderr();
    // std::cout << "Capture stderr: " << capture_error << std::endl;

    if (!bst::containsSubstring(capture_error,
                                "Still capture image received")) {
      capture_failures_++;
      error(DiagnosticId::EXTENSION_piCameraCaptureFail, "nir");
      return;
    }

    capture_failures_ = 0;

    data(data::DataId::CAMERA_newImage, image_file_path);
  } catch (const bst::BoosterSeatException &e) {
    capture_failures_++;
    error(DiagnosticId::EXTENSION_piCameraCaptureFail, e.what());
    return;
  }
}

void PiCamera::shutdown() {
  // nothing to do ¯\_(ツ)_/¯
}

std::string PiCamera::getImageName() {
  constexpr bst::time::TimeZone K_TIME_ZONE = bst::time::TimeZone::UTC;
  constexpr char K_DATE_DELIMITER = '_';
  constexpr char K_BETWEEN_DELIMITER = '_';
  constexpr char K_TIME_DELIMITER = '\0';
  std::string timestamp = bst::time::dateAndTimeString(
      K_TIME_ZONE, K_DATE_DELIMITER, K_BETWEEN_DELIMITER, K_TIME_DELIMITER);

  static const std::string K_IMAGE_EXT = ".jpg";

  return timestamp + K_IMAGE_EXT;
}

} // namespace extension