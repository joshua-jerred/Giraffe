
/**
 * @file module-extensions.cpp
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Defines the ExtensionsModule class.
 * 
 * @version 0.0.9
 * @date 2022-10-11
 * @copyright Copyright (c) 2022
 */
#include <vector>
#include "extensions.h"

#include "modules.h"
using namespace modules;

/**
 * @brief Construct a new ExtensionsModule object.
 * @details Uses the config_data to create each extension and add it to the
 * extension vector. This does not start the extensions.
 * @param config_data 
 * @param stream 
 */
ExtensionsModule::ExtensionsModule(const ConfigData config_data, DataStream *stream):
    Module(stream, MODULE_EXTENSION_PREFIX),
    p_data_stream_(stream),
    config_data_(config_data) {

    for (ExtensionMetadata extdata : config_data.extensions.extensions_list) {
        addExtension(extdata);
    }
}

/**
 * @brief Destroys the ExtensionsModule object.
 */
ExtensionsModule::~ExtensionsModule() {
    for (extension::Extension *ext : extensions_) {
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
    for (extension::Extension *ext : extensions_) {
        ext->start();
    }
}

/**
 * @brief Stops all extensions.
 * @param None
 * @return void
 */
void ExtensionsModule::stop() {
    std::cout << std::endl;
    for (extension::Extension *ext : extensions_) {
        std::cout << "Stopping extension " << ext->getName() << " ... ";
        ext->stop();
        if (ext->getStatus() == ExtensionStatus::STOPPED) {
            std::cout << "Stopped" << std::endl;
        } else {
            std::cout << "Error" << std::endl;
        }
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
        extensions_.push_back(new extension::TestExtension(p_data_stream_, meta_data));
    } else if (meta_data.extension_type == "BMP180_SIM") {
        extensions_.push_back(new extension::BMP180_SIM(p_data_stream_, meta_data));
    } else if (meta_data.extension_type == "SAMM8Q_SIM") {
        extensions_.push_back(new extension::SAMM8Q_SIM(p_data_stream_, meta_data));
    } else if (meta_data.extension_type == "DS18B20_SIM") {
        extensions_.push_back(new extension::DS18B20_SIM(p_data_stream_, meta_data));
    } else if (meta_data.extension_type == "DRA818V_SIM") {
        extensions_.push_back(new extension::DRA818V_SIM(p_data_stream_, meta_data));
    } else if (meta_data.extension_type == "BMP180") {
        extensions_.push_back(new extension::BMP180(p_data_stream_, meta_data));
    } else if (meta_data.extension_type == "DS18B20") {
        extensions_.push_back(new extension::DS18B20(p_data_stream_, meta_data));
    } else if (meta_data.extension_type == "SAMM8Q") {
        extensions_.push_back(new extension::SAMM8Q(p_data_stream_, meta_data));
    } else if (meta_data.extension_type == "BME280") {
        extensions_.push_back(new extension::BME280(p_data_stream_, meta_data));
    } else if (meta_data.extension_type == "SYSINFO") {
        extensions_.push_back(new extension::SYSINFO(p_data_stream_, meta_data));
    } else {
        error("Extension type not found: " + meta_data.extension_type);
    }
}