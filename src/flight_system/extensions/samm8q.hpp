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

#include "extension.hpp"
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
   * @{
   */

  // -- Upper level states -- //
  enum class State { CONFIGURE, RESET, READ };
  // -- Lower level states -- //
  enum class ResetState { SEND_RESET, WAIT, HANDSHAKE };
  enum class ReadState { READ_PACKET, WAIT };

  State state_ = State::CONFIGURE;
  bool transition_in_ = true;
  void transitionState(State new_state);

  void stateConfigure();

  /**
   * @brief Resets the GPS and attempts a handshake.
   * On transition into the reset state, this will send the reset command. After
   * it will attempt to handshake. If it can handshake, it will transition to
   * the configure state. If it can't, it will increment the restart attempts
   * and try again. After hitting the limit (see function definition), it will
   * hard fault the extension.
   */
  void stateReset();

  /**
   * @brief Restart attempts for the reset state.
   * @details This gets cleared with a call to transitionState(reset)
   */
  uint32_t restart_attempts_ = 0;

  void stateRead();

  /** @} */ // end of SAMM8QState

  bool handshake();
  bool resetSensor();

  I2cInterface i2c_;
  BoosterSeat::Timer read_timer_;
};
} // namespace extension

#endif /* SAMM8Q_HPP_ */