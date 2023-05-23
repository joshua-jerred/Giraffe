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

enum class Status {
  UNKNOWN,
  DISABLED,
  STOPPED,
  STARTING,
  RUNNING,
  SLEEPING,
  STOPPING,
  ERROR
};

static const std::map<Status, std::string> status_to_string = {
    {Status::UNKNOWN, "unknown"},   {Status::DISABLED, "disabled"},
    {Status::STOPPED, "stopped"},   {Status::STARTING, "starting"},
    {Status::RUNNING, "running"},   {Status::SLEEPING, "sleeping"},
    {Status::STOPPING, "stopping"}, {Status::ERROR, "error"}};

}  // namespace node

#endif