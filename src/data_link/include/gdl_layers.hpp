/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   gdl_layers.hpp
 * @brief  The network layers for Giraffe Data Link
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2024-01-18
 * @copyright  2024 (license to be defined)
 */

#ifndef GDL_LAYERS_HPP_
#define GDL_LAYERS_HPP_

namespace giraffe::gdl {

class TransportLayer {
public:
  enum class State {
    IDLE,
    SENDING,
    WAITING_FOR_ACK,
  };

  TransportLayer(Config &config, NetworkLayer &network_layer);
  ~TransportLayer();

  bool isReady();

  bool send(Message &message);

  bool send(signal_easel::aprs::PositionPacket &packet);

  bool receive(Message &message);

  void
  update(MessageQueue &received_messages,
         std::queue<signal_easel::aprs::PositionPacket> &aprs_gps_rx_queue);

  void updateStats(Stats &stats);

private:
  bool sendPacket();

  void sendAck(std::string id);

  void startTimer();

  bool isTimerExpired();

  bool receivedAck();

  Config &config_;

  NetworkLayer &network_layer_;

  bst::Timer timer_;

  Message current_message_{};

  State state_ = State::IDLE;

  std::string last_acked_id_{};
};

class NetworkLayer {
public:
  NetworkLayer(Config &config, PhysicalLayer &physical_layer);

  bool txMessage(Message &message);

  bool rxMessage(Message &message);

  bool txAprsPositionPacket(signal_easel::aprs::PositionPacket &packet);

  bool rxAprsPositionPacket(signal_easel::aprs::PositionPacket &packet,
                            signal_easel::ax25::Frame &frame);

  void updateStats(Stats &stats);

  void update();

private:
  Config &config_;

  PhysicalLayer &physical_layer_;
  signal_easel::aprs::Packet base_packet_;
  signal_easel::aprs::Modulator modulator_{};
  signal_easel::aprs::Receiver receiver_{};
};

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

  PhysicalLayer(Config &config);

  ~PhysicalLayer();

  void enable();

  void disable();

  void update();

  PhysicalLayer::State getState() const {
    return state_;
  }

protected:
  Config &config_;

  State state_ = State::DISABLED;
};

} // namespace giraffe::gdl

#endif /* GDL_LAYERS_HPP_ */