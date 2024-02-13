/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   pi_camera.hpp
 * @brief  Raspberry Pi Camera Extension
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-02-12
 * @copyright  2024 (license to be defined)
 */

#ifndef PI_CAMERA_HPP_
#define PI_CAMERA_HPP_

#include "extension_base.hpp"

namespace extension {

/**
 * @brief The PiCamera class is a simple extension that wraps around the
 * libcamera-apps executables.
 * @details This can be much better implemented by replicating the functionality
 * of libcamera-still directly. For now, the exec calls will have to do.
 *
 * @todo Implement with direct implementation instead of wrapping around
 * another executable.
 */
class PiCamera : public Extension {
public:
  PiCamera(ExtensionResources &resources, cfg::ExtensionMetadata metadata);

  void startup() override;
  void loop() override;
  void shutdown() override;

private:
  std::string getImageName();

  /// @brief The directory to save images to.
  std::string image_dir_{};

  /// @brief The number of failed captures.
  uint32_t capture_failures_{0};
};

} // namespace extension

#endif /* PI_CAMERA_HPP_ */