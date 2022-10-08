#include <vector>

#include "module.h"
#include "extension.h"
#include "utility-config-types.h"
#include "utility-data-stream.h"

#include "module-extensions.h"

/*----------------------------------------*/
/* The headers for each extension go here */
#include "simulated-extensions.h"
/*----------------------------------------*/

ExtensionsModule::ExtensionsModule(const ConfigData config_data, DataStream *stream) {
    config_data_ = config_data;
    data_stream_ = stream;
    for (ExtensionMetadata extdata : config_data.extensions.extensions_list) {
        addExtension(extdata);
    }
}

ExtensionsModule::~ExtensionsModule() {
    for (Extension *ext : extensions_) {
        delete ext;
    }
}

void ExtensionsModule::start() {
    for (Extension *ext : extensions_) {
        ext->start();
    }
}

void ExtensionsModule::stop() {
    for (Extension *ext : extensions_) {
        ext->stop();
    }
}

void ExtensionsModule::addExtension(ExtensionMetadata meta_data) {
    if (meta_data.extension_type == "TEST_EXT") {
        extensions_.push_back(new TestExtension(data_stream_, meta_data));
    } else if (meta_data.extension_type == "BMP180_SIM") {
        extensions_.push_back(new BMP180_SIM(data_stream_, meta_data));
    } else if (meta_data.extension_type == "SAMM8Q_SIM") {
        extensions_.push_back(new SAMM8Q_SIM(data_stream_, meta_data));
    } else if (meta_data.extension_type == "DS18B20_SIM") {
        extensions_.push_back(new DS18B20_SIM(data_stream_, meta_data));
    } else if (meta_data.extension_type == "DRA818V_SIM") {
        extensions_.push_back(new DRA818V_SIM(data_stream_, meta_data));
    }
}