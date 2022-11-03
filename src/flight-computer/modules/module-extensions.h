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

#include "module.h"

#include "extension.h"

/*----------------------------------------*/
/* The headers for each extension go here */
#include "simulated-extensions.h"
#include "bmp180.h"
#include "ds18b20.h"
/*----------------------------------------*/

/**
 * @brief The extension class manages all extensions.
 * @details This class is responsible for starting, stopping, and managing
 * extensions. This includes all sensors and data colleciton tools.
 */
class ExtensionsModule : public Module {
public:
    ExtensionsModule(const ConfigData config_data, DataStream *stream);
    ~ExtensionsModule();
    void start();
    void stop();

    //extension_reply command(extension_command command);

private:
    void addExtension(ExtensionMetadata meta_data);

    std::vector<Extension*> extensions_;
    DataStream *data_stream_;
    ConfigData config_data_;
};

#endif