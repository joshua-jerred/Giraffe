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
        mDataStream->addData("TestExt", "TestExt", "100", getUpdateInterval() + 1);
    }
    return 0;
}
// void TestExtension::spawnRunner() {
//     runner_thread_ = std::thread(&TestExtension::runner, this);
// }

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
        mDataStream->addData(getName(), "TF", std::to_string(temp), getUpdateInterval() + 1);
        mDataStream->addData(getName(), "PM", std::to_string(pressure), getUpdateInterval() + 1);
        
    }
    return 0;
}

SAMM8Q_SIM::SAMM8Q_SIM(DataStream *pDS, ExtensionMetadata config_data) :
    Extension(pDS, config_data) {}
SAMM8Q_SIM::~SAMM8Q_SIM() {  
}
int SAMM8Q_SIM::runner() {
    float lat = 37.1010982;
    float lon = -113.5678354;
    int alt = 0;
    std::string quality = "2";
    int vertical_speed = 5;
    int horizontal_speed = 4;
    mDataStream->addData(getName(), "VERT_SPEED", std::to_string(vertical_speed), getUpdateInterval() + 1);
    // ^This should test stale data
    while (true) {
        lat += 0.00001;
        lon += 0.00001;
        alt += 5;
        int horizontal_speed = 2 + (rand() % static_cast<int>(5 - 2 + 1));
        std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
        );
        mDataStream->addData(getName(), "GPS_LAT", std::to_string(lat), getUpdateInterval() + 1);
        mDataStream->addData(getName(), "GPS_LON", std::to_string(lon), getUpdateInterval() + 1);
        mDataStream->addData(getName(), "GPS_ALT", std::to_string(alt), getUpdateInterval() + 1);
        mDataStream->addData(getName(), "GPS_QUAL", quality, getUpdateInterval() + 1);
        mDataStream->addData(getName(), "HORZ_SPEED", std::to_string(horizontal_speed), getUpdateInterval() + 1);
    }
}

DS18B20_SIM::DS18B20_SIM(DataStream *pDS, ExtensionMetadata config_data) :
    Extension(pDS, config_data) {
}
DS18B20_SIM::~DS18B20_SIM() {}
int DS18B20_SIM::runner() {
    int temp = 75;
    int upordown = 1; // 1 = going up, 0 = going down
    while (true) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
        );
        if (upordown == 1) {
            temp -= 1;
        } else {
            temp += 1;
        }
        if (temp == -50) {
            upordown = 0;
        } else if (temp == 75) {
            upordown = 1;
        }
        mDataStream->addData(getName(), "TF", std::to_string(temp), getUpdateInterval() + 1);
    }
}

DRA818V_SIM::DRA818V_SIM(DataStream *pDS, ExtensionMetadata config_data) :
    Extension(pDS, config_data) {
}
DRA818V_SIM::~DRA818V_SIM() { 
}
int DRA818V_SIM::runner() {
    while (true) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(getUpdateInterval())
        );
    }
}