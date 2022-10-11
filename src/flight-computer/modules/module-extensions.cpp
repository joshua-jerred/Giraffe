
/**
 * @file module-extensions.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Defines the ExtensionsModule class.
 * 
 * @version 0.0.9
 * @date 2022-10-11
 * @copyright Copyright (c) 2022
 */

#include "module-extensions.h"

/**
 * @brief Construct a new ExtensionsModule object.
 * @details Uses the config_data to create each extension and add it to the
 * extension vector. This does not start the extensions.
 * @param config_data 
 * @param stream 
 */
ExtensionsModule::ExtensionsModule(const ConfigData config_data, DataStream *stream) {
    config_data_ = config_data;
    data_stream_ = stream;
    for (ExtensionMetadata extdata : config_data.extensions.extensions_list) {
        addExtension(extdata);
    }
}

/**
 * @brief Destroys the ExtensionsModule object.
 */
ExtensionsModule::~ExtensionsModule() {
    for (Extension *ext : extensions_) {
        delete ext;
    }
}

/**
 * @brief Starts the ExtensionsModule.
 * @details Starts each extension in the extension vector.
 * @param None
 * @return void
 */
void ExtensionsModule::start() {
    for (Extension *ext : extensions_) {
        ext->start();
    }
}

/**
 * @brief Stops all extensions.
 * @param None
 * @return void
 */
void ExtensionsModule::stop() {
    for (Extension *ext : extensions_) {
        ext->stop();
    }
}

/**
 * @brief This function uses the extension metadata to create an extension of
 * the correct type.
 * @param meta_data The meta_data for one extension.
 * @return void
 */
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