/**
 * @file test.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Tests for the BMP180 extension.
 * @date 2022-12-19
 * @copyright Copyright (c) 2022
 * @version 0.2.0
 */

#include "gtest/gtest.h"

#include "bmp180.h"
#include "utility-data-stream.h"

class BMP180Test : public ::testing::Test {
protected:
    virtual void SetUp() { 
        p_data_stream_ = new DataStream();

        extension_metadata_.id = 1;
        extension_metadata_.name = "BMP180";
        extension_metadata_.extension_type = "BMP180";
        extension_metadata_.category = ExtensionMetadata::Category::EXTERNAL_SENSOR;
        extension_metadata_.interface = ExtensionMetadata::Interface::I2C;
        extension_metadata_.update_interval = 1000; // time in miliseconds
        extension_metadata_.critical = 0;
        extension_metadata_.extra_args.I2C_bus = 1; // Change this if needed
        extension_metadata_.extra_args.I2C_device_address = 0x77;
     }
    virtual void TearDown() { 
        delete p_data_stream_;
    }
    DataStream *p_data_stream_ = nullptr;
    ExtensionMetadata extension_metadata_ = ExtensionMetadata();
};

TEST_F(BMP180Test, BMP180Test1) {
    BMP180 bmp180(p_data_stream_, extension_metadata_);
    bmp180.start();
    sleep(1);
    bmp180.stop();
    
    int num_data_packets = p_data_stream_->getNumDataPackets();
    if (num_data_packets != 6 || num_data_packets != 22) {
        ADD_FAILURE() << "Expected 6 or 22 (debug mode) data packets, got " << num_data_packets << std::endl;
    }


    if (num_data_packets > 0) {
        std::cout << "Data packets: " << std::endl;
        for (int i = 0; i < num_data_packets; i++) {
            std::cout << p_data_stream_->getNextDataPacket() << std::endl;
        }
    }
    

    int num_error_packets = p_data_stream_->getNumErrorPackets();
    EXPECT_EQ(num_error_packets, 0);
    if (num_error_packets > 0) {
        std::cout << "Error packets: " << std::endl;
        for (int i = 0; i < num_error_packets; i++) {
            std::cout << p_data_stream_->getNextErrorPacket() << std::endl;
        }
    }

}