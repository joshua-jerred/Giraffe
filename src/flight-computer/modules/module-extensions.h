#ifndef MODULE_EXTENSIONS_H_
#define MODULE_EXTENSIONS_H_ 

#include <vector>

#include "extension.h"
#include "module.h"

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