/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Tests for the BME280 extension.
 * @date 2022-12-30
 * @copyright Copyright (c) 2022
 * @version 0.2.0
 */

#include "gtest/gtest.h"

#include "extensions.h"

class BME280Test : public ::testing::Test {
protected:
    virtual void SetUp() {
        p_data_stream_ = new DataStream();

        extension_metadata_.id = 1;
        extension_metadata_.name = "BME280";
        extension_metadata_.extension_type = "BME280";
        extension_metadata_.category = ExtensionMetadata::Category::EXTERNAL_SENSOR;
        extension_metadata_.interface = ExtensionMetadata::Interface::I2C;
        extension_metadata_.update_interval = 1100; // time in milliseconds
        extension_metadata_.critical = 0;
        extension_metadata_.extra_args.I2C_bus = 1; // Change this if needed
        extension_metadata_.extra_args.I2C_device_address = 0x76;
     }
    virtual void TearDown() {
        delete p_data_stream_;
    }
    DataStream *p_data_stream_ = nullptr;
    ExtensionMetadata extension_metadata_ = ExtensionMetadata();
};

TEST_F(BME280Test, BME280Test1) {
    extension::BME280 bme280(p_data_stream_, extension_metadata_);
    bme280.start();
    sleep(1);
    bme280.stop();

    int num_data_packets = p_data_stream_->getNumDataPackets();
    EXPECT_EQ(num_data_packets, 3);
    DataStreamPacket packet;
    for (int i = 0; i < num_data_packets; i++) {
        packet = p_data_stream_->getNextDataPacket();
        EXPECT_EQ(packet.source, extension_metadata_.extension_type);

        if (packet.unit == "TEMP_C") {
            EXPECT_GT(std::stod(packet.value), 10.0) << "Temperature is too low";
            EXPECT_LT(std::stod(packet.value), 40.0) << "Temperature is too high"; // High value due to the sensor being right above the linear regulator
        } else if (packet.unit == "RH_PER") {
            EXPECT_GT(std::stod(packet.value), 0.0) << "Humidity is too low";
            EXPECT_LT(std::stod(packet.value), 100.0) << "Humidity is too high";
        } else if (packet.unit == "PRES_MBAR") {
            EXPECT_GT(std::stod(packet.value), 800.0) << "Pressure is too low";
            EXPECT_LT(std::stod(packet.value), 1100.0) << "Pressure is too high";
        } else {
            ADD_FAILURE() << "Unexpected unit: " << packet.unit << std::endl;
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