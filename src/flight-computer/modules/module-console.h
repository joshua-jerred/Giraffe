#ifndef MODULE_CONSOLE_H_
#define MODULE_CONSOLE_H_


#include <string>

#include "module.h"

#include "utility-config-types.h"
#include "utility-data-stream.h"

class ConsoleModule : public Module {
public:
    ConsoleModule(const ConfigData config_data, DataStream *stream);
    ~ConsoleModule();
    void start();
    void stop();
}