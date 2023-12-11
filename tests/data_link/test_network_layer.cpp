#include "gtest/gtest.h"

#include <BoosterSeat/sleep.hpp>

#include <gdl_network_layer.hpp>

using namespace gdl;

TEST(NetworkLayer, networkLayerTransmit) {
  // SimulatedLoopbackPhysicalLayer physical_layer{};
  // NetworkLayer network_layer(physical_layer);
  // gdl::Message message{1, gdl::Message::Type::UNDEFINED, "test"};
  // EXPECT_TRUE(network_layer.txMessage(message)) << "Failed to transmit
  // message"; if (physical_layer.getState() != PhysicalLayer::State::IDLE) {
  //   BoosterSeat::threadSleep(1);
  //   EXPECT_EQ(physical_layer.getState(), PhysicalLayer::State::TRANSMITTING)
  //       << "Physical layer did not enter TRANSMITTING state but is not at
  //       idle";
  // }
  // EXPECT_EQ(physical_layer.getState(), PhysicalLayer::State::IDLE)
  //     << "Physical layer did not return to IDLE state";
}

TEST(NetworkLayer, networkLayerReceive) {
  // SimulatedLoopbackPhysicalLayer physical_layer{};
  // NetworkLayer network_layer(physical_layer);
  // gdl::Message message{1, gdl::Message::Type::UNDEFINED, "test"};
  // EXPECT_TRUE(network_layer.txMessage(message)) << "Failed to transmit
  // message"; EXPECT_EQ(physical_layer.getState(), PhysicalLayer::State::IDLE)
  //     << "Physical layer did not return to IDLE state";
  // gdl::Message received_message;
  // EXPECT_TRUE(network_layer.rxMessage(received_message))
  //     << "Failed to receive message";
  // EXPECT_EQ(received_message.data, message.data)
  //     << "Received message data does not match transmitted message data";
  // EXPECT_EQ(received_message.id, message.id)
  //    << "Received message id does not match transmitted message id";
}