#include "gtest/gtest.h"

#include <BoosterSeat/sleep.hpp>

// #include <gdl_network_layer.hpp>
// #include <gdl_physical_layer.hpp>
// #include <gdl_transport_layer.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>

// using namespace gdl;

constexpr size_t kMaxMessageSize = 200;
constexpr size_t kMessagesPerTest = 50;
constexpr size_t kMessageTimeoutMs = 50;

// class UnreliablePhysicalLayer : public PhysicalLayer {
// public:
//   void enable() override {
//     state_ = PhysicalLayer::State::IDLE;
//   }
//   void disable() override {
//     state_ = PhysicalLayer::State::DISABLED;
//   }
//   bool transmitBytes(std::vector<uint8_t> &bytes) override {

//     if (state_ == PhysicalLayer::State::IDLE) {
//       state_ = PhysicalLayer::State::TRANSMITTING;
//       received_bytes_ = bytes;
//       state_ = PhysicalLayer::State::IDLE;
//       return true;
//     }
//     return false;
//   }
//   std::vector<uint8_t> &receiveBytes() override {
//     return received_bytes_;
//   }
//   uint16_t getBytesAvailable() override {
//     return received_bytes_.size();
//   }

// private:
//   std::vector<uint8_t> received_bytes_{};

//   double bit_error_rate_ = 0.0;
//   double packet_loss_rate_ = 0.0;
// };

// void sendAndReceive(TransportLayer &transport_layer) {
//   int message_size = std::rand() % kMaxMessageSize;
//   Message message{};
//   static int char_offset = 0;
//   for (int i = 0; i < message_size; i++) {
//     message.data += 'A' + char_offset % 26;
//   }
//   char_offset++;
//   std::string original_message = message.data;

//   EXPECT_TRUE(transport_layer.send(message));
//   EXPECT_TRUE(transport_layer.receive(message));
//   EXPECT_TRUE(message.data == original_message);
// }

TEST(NetworkSimulator, noPhysicalLayerErrors) {
  GTEST_SKIP();
  //   std::cout << std::endl << "Starting GDL Simulator" << std::endl;
  //   BoosterSeat::threadSleep(500);
  //   std::srand(std::time(nullptr));

  //   UnreliablePhysicalLayer physical_layer{};
  //   NetworkLayer network_layer{physical_layer};
  //   TransportLayer transport_layer{network_layer};

  //   for (size_t i = 0; i < kMessagesPerTest; i++) {
  //     sendAndReceive(transport_layer);
  //   }
}