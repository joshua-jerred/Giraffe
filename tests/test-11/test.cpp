/**
 * @file test.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Tests for the BME280 extension.
 * @date 2022-12-30
 * @copyright Copyright (c) 2022
 * @version 0.2.0
 */

#include "gtest/gtest.h"

#include "extensions.h"

class SYSINFOTest : public ::testing::Test {
protected:
    virtual void SetUp() { 
        p_data_stream_ = new DataStream();

        extension_metadata_.id = 1;
        extension_metadata_.name = "sys";
        extension_metadata_.extension_type = "SYSINFO";
        extension_metadata_.category = ExtensionMetadata::Category::INTERNAL_SENSOR;
        extension_metadata_.interface = ExtensionMetadata::Interface::INTERNAL;
        extension_metadata_.update_interval = 1100; // time in miliseconds
        extension_metadata_.critical = 0;
     }
    virtual void TearDown() { 
        delete p_data_stream_;
    }
    DataStream *p_data_stream_ = nullptr;
    ExtensionMetadata extension_metadata_ = ExtensionMetadata();
};

TEST_F(SYSINFOTest, SYSINFOTest1) {
    extension::SYSINFO sysinfo(p_data_stream_, extension_metadata_);
    sysinfo.start();
    sleep(1);
    sysinfo.stop();

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