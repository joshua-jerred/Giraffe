/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   pysical_layer.hpp
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-08-14
 * @copyright  2024 (license to be defined)
 */

#pragma once

#include "gdl_config_and_stats.hpp"

namespace giraffe::gdl {

/// @brief The physical layer of the Giraffe Data Link
class PhysicalLayer {
public:
  enum class State {
    /// @brief The physical layer is in an error state. A call to enable() is
    /// allowed to attempt to recover from this state.
    ERROR,
    /// @brief The physical layer is currently disabled
    DISABLED,
    /// @brief The physical layer is currently starting up
    STARTING,
    /// @brief The physical layer is currently idle and ready to be used
    IDLE,
    /// @brief The physical layer is currently transmitting data
    TRANSMITTING,
    /// @brief The physical layer is currently receiving data
    RECEIVING,
    /// @brief The physical layer is busy and cannot be used. This should be
    /// temporary.
    BUSY
  };

  enum class Mode { NONE, DATA, APRS, SSTV };

  PhysicalLayer(const Config &config) : config_(config) {
  }

  /// @brief Gets the current state of the physical layer
  /// @return The current state of the physical layer
  PhysicalLayer::State getState() const {
    return state_;
  }

  /// @brief Get the current mode of the physical layer
  /// @return The current mode of the physical layer
  Mode getMode() const {
    return mode_;
  }

  virtual ~PhysicalLayer() = default;

  /// @brief Attempts to enable the physical layer
  virtual void enable() = 0;

  /// @brief Attempts to disable the physical layer
  virtual void disable() = 0;

  /// @brief Updates the physical layer
  virtual void update() = 0;

  /// @brief Starts transmitting data if possible
  /// @return \c true if transmission was started, \c false otherwise
  /// @todo add a timeout parameter
  virtual bool startTransmit() = 0;

  /// @brief Stops transmitting data if currently transmitting
  /// @return \c true if a transmission was in progress and was stopped, \c
  /// false otherwise
  virtual bool stopTransmit() = 0;

  /// @brief Gets the current RSSI of the physical layer
  virtual double getRssi() const = 0;

  /// @brief Attempts to change the mode of the physical layer
  /// @param mode - The new mode to change to.
  /// @return \c true if the mode was changed, \c false otherwise
  virtual bool changeMode(Mode mode) = 0;

protected:
  const Config &config_;

  void setState(State state) {
    state_ = state;
  }

  void setMode(Mode mode) {
    mode_ = mode;
  }

private:
  State state_ = State::DISABLED;
  Mode mode_ = Mode::NONE;
};

} // namespace giraffe::gdl