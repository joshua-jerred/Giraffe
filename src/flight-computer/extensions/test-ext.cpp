#include <iostream>
#include <thread>
#include <syncstream>

#include "utility-config-types.h"
#include "utility-data-stream.h"

#include "extension.h"
#include "test-ext.h"

TestExtension::TestExtension(DataStream *pDS, ExtensionMetadata config_data) :
    Extension(pDS, config_data) {
}

TestExtension::~TestExtension() {
}

int TestExtension::runner(std::atomic<bool> stop_flag) {
    std::cout << "This is from the test extension." << std::endl;
    return 0;
}