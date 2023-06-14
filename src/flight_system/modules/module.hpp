/**
 * @file module.h
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Base module
 * @date 2023-05-16
 * @copyright Copyright (c) 2023
 */

#ifndef MODULE_HPP_
#define MODULE_HPP_

#include <atomic>
#include <thread>

#include "command.hpp"
#include "configuration.hpp"
#include "giraffe_assert.hpp"
#include "node.hpp"
#include "shared_data.hpp"

namespace modules {

/**
 * @brief Module metadata
 * @details This struct contains metadata about a module. This includes the
 * module name string, the node identification (used for logging), and the
 * sleep interval (in milliseconds).
 */
struct MetaData {
  MetaData(std::string name, node::Identification id, int sleep_interval = 1000)
      : name_(name), id_(id), sleep_interval_(sleep_interval) {
  }

  const std::string name_;
  const node::Identification id_;
  // data::Source source;
  // command::Destination command_destination;
  int sleep_interval_; // 1 second, default sleep time
};

class Module {
public:
  Module(modules::MetaData &, data::SharedData &, cfg::Configuration &);

  Module(const Module &) = delete;            // No copy constructor
  Module &operator=(const Module &) = delete; // No copy assignment
  virtual ~Module() = default;

  /**
   * @brief Starts a thread to run the module.
   *
   * @details This function will start a thread to run a module, but only if the
   * module is not already running. If the module is already running, an error
   * will be logged. It also resets the stop flag to false.
   */
  void start();

  /**
   * @brief Stops the module by setting the stop flag to true and joining the
   * runner thread.
   *
   * @details This function will stop the module by setting the stop flag to
   * true and joining the runner thread. It also sets the status to STOPPED.
   * If a module is in the sleep state, it will only stop after the next loop.
   * This means that the sleep interval plus the time it takes to run the loop
   * and shutdown will be the total time it takes to stop the module.
   */
  void stop();

  /**
   * @brief Returns the status of the module.
   * @return Returns the status of the module.
   * @see node::Status
   */
  node::Status getStatus() const;

protected:
  /**
   * @brief This function is called when the module is started.
   *
   * @details This is overridden by the module implementation. Called only once
   * at startup.
   *
   */
  virtual void startup() {
  }

  /**
   * @brief Continually called at a rate set by the sleep interval.
   */
  virtual void loop() {
  }

  /**
   * @brief Called only once at shutdown.
   */
  virtual void shutdown() {
  }

  /**
   * @brief Called when a command is received.
   * @param command
   */
  virtual void processCommand(const cmd::Command &command) {
    (void)command;
    giraffe_assert(false);
  }

  virtual void updateLocalConfig() {
    giraffe_assert(false);
  }

  /**
   * @todo Sets the status of the module and reports it to the data stream.
   */
  void setStatus(const node::Status status);

  /**
   * @brief Used internally in the module. This function adds an error to the
   * log stream. It is used to log errors that occur in the module.
   *
   * @details Uses the module meta data to add the correct node identification.
   *
   * @param log_id The log id of the error.
   * @param info Optional information to add to the log.
   */
  void error(data::LogId, std::string info = "");

  /**
   * @brief Same as the other error function, but takes an int instead of a
   * string.
   */
  void error(data::LogId log_id, int info);

  /**
   * @brief Used internally in the module. This function adds a info message to
   * the log stream.
   *
   * @details Uses the module meta data to add the correct node identification.
   *
   * @param info info string to add to the log.
   */
  void info(std::string info = "");

  /**
   * @brief Used internally in the module. This function adds a data to the
   * data stream.
   *
   * @tparam T a non-string type that can be converted to a string with
   * std::to_string
   * @param identifier The data identifier.
   * @param value The value of the data.
   * @param precision The precision (if the type is a float or double)
   */
  template <typename T>
  void data(data::DataId identifier, T value, int precision = 2);

  /**
   * @brief Returns true if the stop flag is set to true.
   *
   * @details Used internally in a module to check if the module should stop
   * in between large tasks.
   *
   * @return true
   * @return false
   */
  bool stopRequested() const {
    return stop_flag_;
  }

  /**
   * @brief Sleeps the module for the sleep interval.
   *
   * @details Default of 1 second, this is blocking.
   */
  void sleep();

  modules::MetaData metadata_;
  data::SharedData &shared_data_;
  cfg::Configuration &configuration_;

private:
  /**
   * @brief This is the main function for each module.
   * @details This function is what runs in the thread. It calls startup, then
   * loops until the stop flag is set to true. It then calls shutdown.
   *
   * It also handles the command queue.
   */
  void runner();

  /**
   * @brief The status of the module.
   * @see node::Status
   */
  std::atomic<node::Status> status_ = node::Status::UNKNOWN;

  /**
   * @brief The stop flag is used to stop the module.
   * @details Set to true to stop the module upon a call to stop().
   */
  std::atomic<bool> stop_flag_ = true;

  /**
   * @brief The thread that runs runner().
   */
  std::thread runner_thread_;

  /**
   * @brief The command queue for the module.
   * @see cmd::CommandQueue
   */
  cmd::CommandQueue command_queue_;
};

} // namespace modules
#endif