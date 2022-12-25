/**
 * @file module-extensions.h
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Declares the ExtensionsModule class.
 * 
 * @version 0.0.9
 * @date 2022-10-11
 * @copyright Copyright (c) 2022
 */

#ifndef MODULE_EXTENSIONS_H_
#define MODULE_EXTENSIONS_H_ 

#include <vector>

#include "utility-config-types.h"
#include "utility-data-stream.h"

#include "modules.h"

#include "extensions.h"

/**
 * @brief The extension class manages all extensions.
 * @details This class is responsible for starting, stopping, and managing
 * extensions. This includes all sensors and data collection tools.
 */
class ExtensionsModule : public Module {
public:
    ExtensionsModule(const ConfigData config_data, DataStream *stream);
    ExtensionsModule(const ExtensionsModule &other) = delete; // No copy constructor
    ExtensionsModule &operator=(const ExtensionsModule &other) = delete; // No copy assignment
    ~ExtensionsModule();
    void start();
    void stop();

    //extension_reply command(extension_command command);

private:
    void addExtension(ExtensionMetadata meta_data);

    std::vector<Extension*> extensions_ = {};
    DataStream *p_data_stream_;
    ConfigData config_data_;
};

#endif