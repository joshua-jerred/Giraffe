#include "test.h"
#include "shared_data.hpp"
#include "configuration.hpp"



class ConfigurationTest : public ::testing::Test {
protected:
    virtual void SetUp() { 
    }
    virtual void TearDown() { 
    }
};

TEST_F(ConfigurationTest, test1) {
    data::SharedData shared_data = data::SharedData();
    
    cfg::Configuration config = {shared_data.streams};
}