#ifndef NODE_H_
#define NODE_H_

#include <map>

namespace node {

enum class Identification {
  UNKNOWN,
  CONFIGURATION,
  DATA_MODULE,
  CONSOLE_MODULE,
  SERVER_MODULE,
  SYSTEM_MODULE
};

static const std::map<Identification, std::string> identification_to_string = {
    {Identification::UNKNOWN, "unknown"},
    {Identification::CONFIGURATION, "configuration"},
    {Identification::DATA_MODULE, "data_module"},
    {Identification::CONSOLE_MODULE, "console_module"},
    {Identification::SERVER_MODULE, "server_module"},
    {Identification::SYSTEM_MODULE, "system_module"}};


}  // namespace node

#endif