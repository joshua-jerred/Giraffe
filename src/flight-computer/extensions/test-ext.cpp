#include <iostream>
#include <thread>
#include <syncstream>
#include <atomic>

#include "utility-config-types.h"
#include "utility-data-stream.h"

#include "extension.h"

#include "test-ext.h"

TestExtension::TestExtension(DataStream *pDS, ExtensionMetadata config_data) :
    Extension(pDS, config_data) {
}

TestExtension::~TestExtension() {
}

int TestExtension::runner() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        std::cout << "TestExt" << std::endl;
        mDataStream->addData("TestExt", "TestExt", "100");
    }
    return 0;
}

void TestExtension::spawnRunner() {
    runner_thread_ = std::thread(&TestExtension::runner, this);
}