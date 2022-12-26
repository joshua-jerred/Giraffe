/**
 * @file test.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Tests for the Extension Module
 * @date 2022-11-03
 * @copyright Copyright (c) 2022
 * @version 0.1.0
 */

#include <string>
#include <chrono>
#include <thread>

#include "gtest/gtest.h"

#include "utility-data-stream.h"
#include "modules.h"


class ExtensionsModuleTests : public ::testing::Test {
protected:
    virtual void SetUp() {
        data_stream_ = new DataStream();
    }
    virtual void TearDown() {

    }
    DataStream *data_stream_;
    modules::ExtensionsModule *extensions_module_;
};

TEST_F(ExtensionsModuleTests, StartAndStop) {

}