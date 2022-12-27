/**
 * @file test.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Tests for the I2C Utility
 * @date 2022-12-26
 * @copyright Copyright (c) 2022
 * @version 0.1.0
 */

#include "gtest/gtest.h"
#include "ubx.h"

#define BUS_NUMBER 1
#define DEVICE_ADDRESS 0x42

std::ostream& operator<<(std::ostream& o, const ubx::UBXMessage& ubx) {
    o << "UBX - Class: 0x" << std::hex << (int)ubx.classID;
    o << " ID: 0x" << std::hex << (int)ubx.msgID;
    o << " Length: " << std::dec << ubx.length;
    o << " Payload: ";
    for (int i = 0; i < ubx.length; i++) {
        o << std::hex << (int)ubx.payload[i] << " ";
    }
    o << " ck_a: 0x" << std::hex << (int)ubx.ck_a;
    o << " ck_b: 0x" << std::hex << (int)ubx.ck_b;
    o << std::endl;
    return o;
}

class UBXTest : public ::testing::Test {
   protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(UBXTest, UBXMessageTest) {
    // Test the constructor with known payload (CFG-PRT)
    uint8_t class_id = 0x06;
    uint8_t message_id = 0x00;
    uint8_t payload_length = 20;
    uint8_t payload[] = {
        0x00,                    // portID
        0x00,                    // reserved0
        0x00, 0x00,              // txReady
        0x84, 0x00, 0x00, 0x00,  // mode (0x42 << 1, little endian)
        0x00, 0x00, 0x00, 0x00,  // reserved1
        0x01, 0x00,              // inProtoMask (UBX only)
        0x01, 0x00,              // outProtoMask (UBX only)
        0x00, 0x00,              // flags (no extended timeout)
        0x00, 0x00               // reserved2
    };

    ubx::UBXMessage message(class_id, message_id, payload_length, payload);
    // std::cout << message << std::endl;

    // Check the class_id, message_id, payload_length, and checksum
    EXPECT_EQ(message.classID, class_id);
    EXPECT_EQ(message.msgID, message_id);
    EXPECT_EQ(message.length, payload_length);
    EXPECT_EQ(message.ck_a, 0xA0);
    EXPECT_EQ(message.ck_b, 0x96);
}

TEST_F(UBXTest, UBXSetDDCandAckTest) {
    // Test the setProtocolDDC function
    I2C i2c(BUS_NUMBER, DEVICE_ADDRESS);
    i2c.connect();
    ASSERT_EQ(i2c.status(), I2C_STATUS::OK) << "I2C connection failed";

    int stream_size = ubx::getStreamSize(i2c);
    ASSERT_EQ(stream_size, 0)
        << "Stream size is not 0. Reset the device or wait for timeout";

    bool extended_timeout = false;
    ubx::ACK result = ubx::setProtocolDDC(i2c, extended_timeout);
    ASSERT_NE(result, ubx::ACK::WRITE_ERROR) << "Write error";
    EXPECT_NE(result, ubx::ACK::NACK) << "NACK received";
    EXPECT_NE(result, ubx::ACK::NONE) << "No ACK Found";
    EXPECT_EQ(result, ubx::ACK::ACK) << "ACK not received";
}

TEST_F(UBXTest, UBXGenerateAndDetectNAK) {
    uint8_t class_id = 0x06;   // CFG
    uint8_t message_id = 0x00; // PRT
    uint16_t payload_length = 10; // Invalid payload length
    uint8_t payload[] = {
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00
    };

    ubx::UBXMessage message(class_id, message_id, payload_length, payload);
    
    I2C i2c(BUS_NUMBER, DEVICE_ADDRESS);
    i2c.connect();
    ASSERT_EQ(i2c.status(), I2C_STATUS::OK) << "I2C connection failed";

    bool result = ubx::writeUBX(i2c, message);
    ASSERT_TRUE(result) << "Write error";

    ubx::ACK ack = ubx::checkForAck(i2c, class_id, message_id);
    EXPECT_NE(ack, ubx::ACK::NONE) << "No ACK Found";
    EXPECT_NE(ack, ubx::ACK::ACK) << "ACK received";
    EXPECT_EQ(ack, ubx::ACK::NACK) << "NACK not received";
}

TEST_F(UBXTest, UBXDetectWriteError) {
    I2C i2c(0x00, 0x00); // Invalid bus number
    i2c.connect();
    ASSERT_EQ(i2c.status(), I2C_STATUS::CONFIG_ERROR_BUS) << "I2C connection did not fail as expected";
    ubx::ACK ack = ubx::setProtocolDDC(i2c, false);
    EXPECT_EQ(ack, ubx::ACK::WRITE_ERROR) << "Write error not detected";
}

TEST_F(UBXTest, UBXNoACK) {
    I2C i2c(BUS_NUMBER, DEVICE_ADDRESS);
    i2c.connect();
    ASSERT_EQ(i2c.status(), I2C_STATUS::OK) << "I2C connection failed";

    ubx::ACK ack = ubx::checkForAck(i2c, 0x00, 0x00);
    EXPECT_EQ(ack, ubx::ACK::NONE) << "ACK found";
}