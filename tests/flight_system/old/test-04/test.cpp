/**
 * @file test.cpp
 * @author Joshua Jerred (https://joshuajer.red/)
 * @brief Tests for the Extension Module
 * @date 2022-11-03
 * @copyright Copyright (c) 2022
 * @version 0.1.0
 */

#include <string>
#include <chrono>
#include <thread>

#include "gtest/gtest.h"

#include "data-stream.h"
#include "modules.h"


class ExtensionsModuleTests : public ::testing::Test {
protected:
    virtual void SetUp() {
        data_stream_ = new DataStream();
    }
    virtual void TearDown() {

    }
    DataStream *data_stream_ = nullptr;
    modules::ExtensionsModule *extensions_module_ = nullptr;
};

TEST_F(ExtensionsModuleTests, StartAndStop) {

}