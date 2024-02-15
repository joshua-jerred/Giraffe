/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Data Stream utility tests
 *
 * @version 0.1.0
 * @date 2022-09-29
 * @copyright Copyright (c) 2022
 */

#include <string>
#include <time.h>

#include "gtest/gtest.h"

#include "data-stream.h"

class DataStream_Utility : public ::testing::Test {
protected:
    virtual void SetUp() {
        p_data_stream_ = new DataStream();
     }
    virtual void TearDown() {
        delete p_data_stream_;
    }
    DataStream *p_data_stream_ = NULL;
};

TEST_F(DataStream_Utility, DataPackets) {
    DataStreamPacket packet1;
    DataStreamPacket packet2;

    packet1.source = "1test";
    packet1.unit = "1unit";
    packet1.value = "1value";
    packet1.expiration_time = 5; // Seconds until expiration
    int current_time = time(NULL);
    int expected_expiration_time = current_time + packet1.expiration_time;
    p_data_stream_->addData(
        packet1.source,
        packet1.unit,
        packet1.value,
        packet1.expiration_time
    );

    ASSERT_EQ(p_data_stream_->getNumDataPackets(), 1) << "Number of data packets is incorrect";
    ASSERT_EQ(p_data_stream_->getTotalDataPackets(), 1) << "Total data packets incorrect";

    packet2 = p_data_stream_->getNextDataPacket();

    EXPECT_EQ(packet1.source, packet2.source) << "Source is incorrect";
    EXPECT_EQ(packet1.unit, packet2.unit) << "Unit is incorrect";
    EXPECT_EQ(packet1.value, packet2.value) << "Value is incorrect";
    EXPECT_EQ(expected_expiration_time, packet2.expiration_time) << "Expiration time is incorrect";

    EXPECT_EQ(p_data_stream_->getNumDataPackets(), 0) << "Number of data packets is incorrect";
    EXPECT_EQ(p_data_stream_->getTotalDataPackets(), 1) << "Total data packets incorrect (should not have changed)";
}

TEST_F(DataStream_Utility, DataFrame) {
    DataFrame frame;

    DataStreamPacket packet_input;

    packet_input.source = "1test";
    packet_input.unit = "1unit";
    packet_input.value = "1value";
    packet_input.expiration_time = 5;
    frame.insert_or_assign(packet_input.source + ":" + packet_input.unit, packet_input);

    p_data_stream_->updateDataFrame(frame);

    EXPECT_EQ(p_data_stream_->getData(packet_input.source, packet_input.unit), packet_input.value) << "Value is incorrect";
}