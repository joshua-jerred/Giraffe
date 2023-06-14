/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   extension.hpp
 * @brief  The new implementation of the extension system.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-12
 * @copyright  2023 (license to be defined)
 */

#ifndef EXTENSION_HPP_
#define EXTENSION_HPP_

#include <atomic>
#include <thread>

#include "sections/cfg_extensions.hpp"
#include "shared_data.hpp"

namespace ext {

/**
 * @brief This contains objects that are shared between all extensions.
 */
struct ExtensionInterfaces {
  ExtensionInterfaces(data::Streams &streams) : streams(streams) {
  }
  data::Streams &streams;
};

/**
 * @brief
 * @details Many of the things in here are nearly identical to
 * modules. However, there are few subtle differences.
 *
 * Modules are unique, you will only every have one "server" module.
 * Extensions are not. You may have multiple BMP280 sensors, for example.
 * As extensions are identified by name, the handling of the data
 * packets is a bit different. Along with that, extensions do not have
 * direct access to 'shared_data' as modules do. Instead, they all have
 * access to the data/log stream, but only through the data(), error(),
 * and info() functions.
 *
 * Extensions do not need to be written in a thread safe manner as they
 * are run in their own thread and their only interface is through
 * methods that are thread safe.
 */
class Extension {
public:
  Extension(ExtensionInterfaces &extension_interfaces,
            cfg::ExtensionMetadata metadata);
  Extension(const Extension &) = delete;            // no copy constructor
  Extension &operator=(const Extension &) = delete; // no copy assignment

  /**
   * @brief This will attempt to stop the extension if it is running, but with
   * a configured timeout. @see kExtensionDestroyTimeoutMs
   *
   * @details This really shouldn't be used, but it is here to silence a
   * warning and it's there if needed.
   */
  virtual ~Extension();

  /**
   * @brief Starts the extension.
   */
  void start();

  /**
   * @brief Sends a stop signal to the extension.
   */
  void stop();

  /**
   * @brief Get the Status of the extension.
   * @return node::Status
   */
  node::Status getStatus() const;

protected:
  virtual void startup(){};
  virtual void loop(){};
  virtual void shutdown(){};

  void setStatus(node::Status status);
  void error(data::LogId, const std::string &info = "");
  void error(data::LogId log_id, int info);
  void info(std::string info);

  template <typename T>
  void data(data::DataId identifier, T value, int precision = 2);

  bool stopRequested() const;

  cfg::ExtensionMetadata metadata_;

private:
  void sleep();
  void runner();

  std::atomic<node::Status> status_ = node::Status::UNKNOWN;
  std::atomic<bool> stop_flag_ = true;
  std::thread runner_thread_ = std::thread();

  ExtensionInterfaces &interfaces_;
};
} // namespace ext

#endif /* EXTENSION_HPP_ */