#include <iostream>
#include <thread>
#include <syncstream>
#include <atomic>

#include "utility-config-types.h"
#include "utility-data-stream.h"

#include "extension.h"

#include "simulated-extensions.h"

TestExtension::TestExtension(DataStream *pDS, ExtensionMetadata config_data) :
    Extension(pDS, config_data) {
}
TestExtension::~TestExtension() {
}
int TestExtension::runner() {
    while (true) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
            );
        std::cout << "TestExt" << std::endl;
        mDataStream->addData("TestExt", "TestExt", "100");
    }
    return 0;
}
void TestExtension::spawnRunner() {
    runner_thread_ = std::thread(&TestExtension::runner, this);
}


BMP180_SIM::BMP180_SIM(DataStream *pDS, ExtensionMetadata config_data) :
    Extension(pDS, config_data) {
}
BMP180_SIM::~BMP180_SIM() {
}
int BMP180_SIM::runner() {
    int temp = 75;
    int pressure = 1018;
    int upOrDown = 1; // 1 = going up, 0 = going down
    while (true) {

        if (upOrDown == 1) {
            temp -= 1;
            pressure -= 7;
        } else {
            temp += 1;
            pressure += 1;
        }

        if (temp == -50) {
            upOrDown = 0;
        } else if (temp == 75) {
            upOrDown = 1;
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
            );
        mDataStream->addData(getName(), "TF", std::to_string(temp));
        mDataStream->addData(getName(), "PM", std::to_string(pressure));
        
    }
    return 0;
}
void BMP180_SIM::spawnRunner() {
    runner_thread_ = std::thread(&BMP180_SIM::runner, this);
}