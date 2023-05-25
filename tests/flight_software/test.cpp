#include <string>
#include <time.h>

#include "gtest/gtest.h"

class Test1 : public ::testing::Test {
protected:
    virtual void SetUp() { 
     }
    virtual void TearDown() { 
    }
};

TEST_F(Test1, test1) {

}