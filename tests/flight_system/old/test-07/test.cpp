/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Tests for the ds18b20 extension.
 * @date 2022-12-20
 * @copyright Copyright (c) 2022
 * @version 0.3
 */

#include "gtest/gtest.h"

#include "extensions.h"

#define TEST_GOOD_DEVICE_ID "28-03176038beff" // Actual device
#define TEST_BAD_DEVICE_ID  "28-03176038aaaa" // Fake device

class DS18B20Test : public ::testing::Test {
protected:
    virtual void SetUp() {
        p_data_stream_ = new DataStream();

        extension_metadata_.id = 1;
        extension_metadata_.name = "DRA818";
        extension_metadata_.extension_type = "DRA818";
        extension_metadata_.category = ExtensionMetadata::Category::EXTERNAL_SENSOR;
        extension_metadata_.interface = ExtensionMetadata::Interface::ONEWIRE;
        extension_metadata_.update_interval = 1100; // time in milliseconds
        extension_metadata_.critical = 0;
        extension_metadata_.extra_args.one_wire_id = TEST_GOOD_DEVICE_ID;
     }
    virtual void TearDown() {
        delete p_data_stream_;
    }
    DataStream *p_data_stream_ = nullptr;
    ExtensionMetadata extension_metadata_ = ExtensionMetadata();
};

TEST_F(DS18B20Test, OneWireGoodDevice) {
    OneWire one_wire = OneWire(TEST_GOOD_DEVICE_ID);
    EXPECT_EQ(one_wire.status(), ONEWIRE_STATUS::OK);
    std::string result = one_wire.read_w1_slave();
    EXPECT_GT(result.size(), 60);
    EXPECT_LT(result.size(), 80);
    //std::cout << "Result: " << result << "Length: " << result.size() << std::endl;
    std::string temp = "";
    if (result.find("t=") != std::string::npos) {
        temp = result.substr(result.find("t=") + 2);
    } else {
        ADD_FAILURE() << "Could not find t= in w1_slave";
    }
    //std::cout << "Temp: " << temp << std::endl;
    int temp_1 = std::stoi(temp);
    int temp_2 = std::stoi(one_wire.read_temperature());

    if (temp_1 - temp_2 > 2 || temp_2 - temp_1 > 2) {
        ADD_FAILURE() << "Temperature read from w1_slave and read_temperature() are not within 2" + std::to_string(temp_1) + " " + std::to_string(temp_2);
    }

    std::cout << "Temp: " << (float) temp_1 / 1000.0 << std::endl;
}

TEST_F(DS18B20Test, OneWireBadDevice) {
    OneWire one_wire = OneWire(TEST_BAD_DEVICE_ID);
    EXPECT_EQ(one_wire.status(), ONEWIRE_STATUS::NOT_FOUND);
    std::string result = one_wire.read_w1_slave();
    EXPECT_EQ(result, "");
    std::string temp = one_wire.read_temperature();
    EXPECT_EQ(temp, "");

    EXPECT_EQ(one_wire.status(), ONEWIRE_STATUS::READ_ERROR);
}

TEST_F(DS18B20Test, DS18B20Test) {
    extension::DS18B20 ds18b20(p_data_stream_, extension_metadata_);
    ds18b20.start();
    sleep(1);
    ds18b20.stop();

    int num_data_packets = p_data_stream_->getNumDataPackets();
    EXPECT_EQ(num_data_packets, 2);

    std::cout << "Data packets: " << std::endl;
    for (int i = 0; i < num_data_packets; i++) {
        DataStreamPacket packet = p_data_stream_->getNextDataPacket();
        std::cout << packet << std::endl;
        if (packet.unit == "TEMP_C") {
            EXPECT_GT(std::stod(packet.value), 10.0);
            EXPECT_LT(std::stod(packet.value), 33);
        }
        if (packet.unit == "TEMP_F") {
            EXPECT_GT(std::stod(packet.value), 50);
            EXPECT_LT(std::stod(packet.value), 90);
        }
    }

    int num_error_packets = p_data_stream_->getNumErrorPackets();
    EXPECT_EQ(num_error_packets, 0);
    if (num_error_packets > 0) {
        std::cout << "Error packets: " << std::endl;
        for (int i = 0; i < num_error_packets; i++) {
            ErrorStreamPacket packet = p_data_stream_->getNextErrorPacket();
            std::cout << packet << std::endl;
        }
    }
}