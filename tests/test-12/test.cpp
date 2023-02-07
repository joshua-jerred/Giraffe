/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Tests for the MCP3021 extension
 * @date 2022-12-30
 * @copyright Copyright (c) 2022
 * @version 0.3
 */

#include "gtest/gtest.h"

#include "extensions.h"

class mcp3021Test : public ::testing::Test {
protected:
    virtual void SetUp() { 
        p_data_stream_ = new DataStream();

        extension_metadata_.id = 1;
        extension_metadata_.name = "mcp3021";
        extension_metadata_.extension_type = "MCP3021";
        extension_metadata_.category = ExtensionMetadata::Category::EXTERNAL_SENSOR;
        extension_metadata_.interface = ExtensionMetadata::Interface::I2C;
        extension_metadata_.update_interval = 1100; // time in miliseconds
        extension_metadata_.critical = 0;
        extension_metadata_.extra_args.I2C_bus = 1;
     }
    virtual void TearDown() { 
        delete p_data_stream_;
    }
    DataStream *p_data_stream_ = nullptr;
    ExtensionMetadata extension_metadata_ = ExtensionMetadata();
};

TEST_F(mcp3021Test, mcp3021Test1) {
    extension::MCP3021 mcp3021(p_data_stream_, extension_metadata_);
    mcp3021.start();
    sleep(1);
    mcp3021.stop();

    int num_data_packets = p_data_stream_->getNumDataPackets();
    EXPECT_EQ(num_data_packets, 11);
    DataStreamPacket packet;
    for (int i = 0; i < num_data_packets; i++) {
        packet = p_data_stream_->getNextDataPacket();
        EXPECT_EQ(packet.source, extension_metadata_.name);
        //std::cout << packet << std::endl;
    }

    int num_error_packets = p_data_stream_->getNumErrorPackets();
    EXPECT_EQ(num_error_packets, 0);
    if (num_error_packets > 0) {
        std::cout << "Error packets: " << std::endl;
        for (int i = 0; i < num_error_packets; i++) {
            std::cout << p_data_stream_->getNextErrorPacket();
        }
    }
}