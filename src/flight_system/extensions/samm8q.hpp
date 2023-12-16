/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   samm8q.hpp
 * @brief  SAM-M8Q GPS extension (I2C).
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-07-04
 * @copyright  2023 (license to be defined)
 */

#ifndef SAMM8Q_HPP_
#define SAMM8Q_HPP_

#include <BoosterSeat/timer.hpp>

#include "extension_base.hpp"
#include "i2c_interface.hpp"
#include "positional.hpp"

namespace extension {
class SamM8qExtension : public Extension {
public:
  SamM8qExtension(ExtensionResources &resources,
                  cfg::ExtensionMetadata metadata);

  /**
   * @brief Startup the extension.
   * @details For the SAM-M8Q, startup first tries to connect to the I2C bus. If
   * it is able to do this, it will handshake with the sensor to verify that
   * it's there. Configuration does not happen until the loop function. If it
   * can connect to the I2C interface and handshake, it will do nothing.
   * However, if it's not able to do either of these things the startup function
   * will hard fault the extension.
   */
  void startup() override;

  /**
   * @brief Contains a simple state machine that will configure, reset, and read
   * data from the sensor. The I2C interface with the sensor is a bit of a
   * challenge, being able to reset it dynamically is helpful when needed.
   */
  void loop() override;

  /**
   * @brief Sends basic commands to the sensor to get it ready for the next
   * startup.
   */
  void shutdown() override;

private:
  /**
   * @brief The SAM-M8Q state machine.
   * @defgroup SAMM8QState SAM-M8Q State Machine
   * @see sam_m8q_gps_state_machine.puml
   * @{
   */

  // -- Upper level states -- //
  enum class State { CONFIGURE, RESET, READ };
  // -- Lower level states -- //
  enum class ResetState { SEND_RESET, WAIT, HANDSHAKE };
  enum class ReadState { READ_PACKET, WAIT };

  State state_ = State::CONFIGURE;
  ResetState reset_state_ = ResetState::SEND_RESET;
  ReadState read_state_ = ReadState::WAIT;

  void transitionState(State new_state);

  void stateConfigure();
  void stateReset();
  void stateRead();

  /** @} */ // end of SAMM8QState

  bool handshake();
  bool resetSensor();

  I2cInterface i2c_;

  /**
   * @brief This timer should have the largest value of all timers.
   */
  bst::Timer primary_watchdog_timer_;

  /**
   * @brief
   */
  bst::Timer read_watchdog_timer_;

  /**
   * @brief The timer used to give the sensor time to reset after the reset
   * command is sent.
   */
  bst::Timer reset_wait_timer_;

  /**
   * @brief Use to keep track of how many times we have attempted to configure
   * the sensor.
   * @details This is reset to 0 after successful configuration. The maximum is
   * defined in the CPP file.
   */
  uint32_t configure_attempts_ = 0;
  uint32_t reset_attempts_ = 0;
};

} // namespace extension

#endif /* SAMM8Q_HPP_ */