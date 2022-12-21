/**
 * @file test.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Tests for the ds18b20 extension.
 * @date 2022-12-20
 * @copyright Copyright (c) 2022
 * @version 0.2.0
 */

#include "gtest/gtest.h"

#include "ds18b20.h"
#include "utility-data-stream.h"
#include "utility-one-wire.h"

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
        extension_metadata_.update_interval = 1000; // time in miliseconds
        extension_metadata_.critical = 0;
        extension_metadata_.extra_args.one_wire_id = "";
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
    std::cout << "Result: " << result << std::endl;
    //std::string temp = result.substr(result.find("t=") + 2);

}

TEST_F(DS18B20Test, OneWireBadDevice) {
    OneWire one_wire = OneWire(TEST_BAD_DEVICE_ID);
    EXPECT_EQ(one_wire.status(), ONEWIRE_STATUS::OK);
}

TEST_F(DS18B20Test, DS18B20Test1) {
    DS18B20 ds18b20(p_data_stream_, extension_metadata_);
    //ds18b20.start();
    //sleep(1);
    //ds18b20.stop();
}