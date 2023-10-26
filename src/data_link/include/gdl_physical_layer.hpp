/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_physical_layer.hpp
 * @brief  The generic physical layer for the Giraffe Data Link
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-10-26
 * @copyright  2023 (license to be defined)
 */

#ifndef GDL_PHYSICAL_LAYER_HPP_
#define GDL_PHYSICAL_LAYER_HPP_

#include <cstdint>
#include <vector>

namespace gdl {

class PhysicalLayer {
public:
  enum class State {
    ERROR,
    DISABLED,
    STARTING,
    IDLE,
    TRANSMITTING,
    RECEIVING,
    BUSY
  };

  virtual ~PhysicalLayer() = default;

  virtual void enable() = 0;
  virtual void disable() = 0;

  PhysicalLayer::State getState() const {
    return state_;
  }

  virtual bool transmitBytes(std::vector<uint8_t> &bytes) = 0;
  virtual std::vector<uint8_t> &receiveBytes() = 0;
  virtual uint16_t getBytesAvailable() = 0;

protected:
  State state_ = State::DISABLED;
};

/**
 * @brief A simple simulated physical layer for testing.
 */
class SimulatedLoopbackPhysicalLayer : public PhysicalLayer {
public:
  void enable() override {
    state_ = PhysicalLayer::State::IDLE;
  }
  void disable() override {
    state_ = PhysicalLayer::State::DISABLED;
  }
  bool transmitBytes(std::vector<uint8_t> &bytes) override {
    if (state_ == PhysicalLayer::State::IDLE) {
      state_ = PhysicalLayer::State::TRANSMITTING;
      received_bytes_ = bytes;
      state_ = PhysicalLayer::State::IDLE;
      return true;
    }
    return false;
  }
  std::vector<uint8_t> &receiveBytes() override {
    return received_bytes_;
  }
  uint16_t getBytesAvailable() override {
    return received_bytes_.size();
  }

private:
  std::vector<uint8_t> received_bytes_{};
};

} // namespace gdl

#endif /* GDL_PHYSICAL_LAYER_HPP_ */