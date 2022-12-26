/**
 * @file test.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Tests for the Data Module
 * @date 2022-11-03
 * @copyright Copyright (c) 2022
 * @version 0.0.9
 */

#include <string>
#include <chrono>
#include <thread>

#include "gtest/gtest.h"

#include "modules.h"


class DataModuleTests : public ::testing::Test {
protected:
    virtual void SetUp() {
        data_module_ = new modules::DataModule();
    }
    virtual void TearDown() {

    }
    modules::DataModule *data_module_;
};

TEST_F(DataModuleTests, StartAndStop) {
    EXPECT_EQ(ModuleStatus::STOPPED, data_module_->status())
        << "Data module is not stopped";
    data_module_->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Delay to allow the thread to start
    EXPECT_EQ(ModuleStatus::RUNNING, data_module_->status())
        << "Data module is not running";
    data_module_->stop();
    EXPECT_EQ(ModuleStatus::STOPPED, data_module_->status())
        << "Data module is not stopped";
}

TEST_F(DataModuleTests, DataStreamTests) {
    DataStream *stream = data_module_->getDataStream();
    EXPECT_EQ(0, stream->getNumDataPackets())
        << "Data stream is not empty";
    EXPECT_EQ(0, stream->getNumErrorPackets())
        << "Data stream is not empty";
    
    stream->addData("source", "name", "value", 0);

    EXPECT_EQ(1, stream->getNumDataPackets())
        << "Data stream is empty";
    EXPECT_EQ(0, stream->getNumErrorPackets())
        << "Error stream is not empty";

    stream->addError("source", "name", "info", 0);

    data_module_->start();
    /* Delay to allow the thread to start and process the data packet.*/
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    EXPECT_EQ(0, stream->getNumDataPackets())
        << "Data stream is not empty";
    EXPECT_EQ(0, stream->getNumErrorPackets())
        << "Error stream is not empty";
}