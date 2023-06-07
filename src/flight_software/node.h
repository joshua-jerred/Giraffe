#ifndef NODE_H_
#define NODE_H_

#include <map>

namespace node {

/**
 * @brief Node identification, used for logging, telemetry, command routing,
 * and more.
 */
enum class Identification {
  UNKNOWN,
  FLIGHT_RUNNER,
  CONFIGURATION,
  DATA_MODULE,
  CONSOLE_MODULE,
  SERVER_MODULE,
  SYSTEM_MODULE
};

/**
 * @brief Map of node identification to string
 * @see node::Identification
 */
static const std::map<Identification, std::string> identification_to_string = {
    {Identification::UNKNOWN, "unknown"},
    {Identification::FLIGHT_RUNNER, "flight_runner"},
    {Identification::CONFIGURATION, "configuration"},
    {Identification::DATA_MODULE, "data_module"},
    {Identification::CONSOLE_MODULE, "console_module"},
    {Identification::SERVER_MODULE, "server_module"},
    {Identification::SYSTEM_MODULE, "system_module"}};

/**
 * @brief Node status, used for modules and extensions.
 */
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

/**
 * @brief Map of node status to string
 * @see node::Status
 */
static const std::map<Status, std::string> status_to_string = {
    {Status::UNKNOWN, "unknown"},   {Status::DISABLED, "disabled"},
    {Status::STOPPED, "stopped"},   {Status::STARTING, "starting"},
    {Status::RUNNING, "running"},   {Status::SLEEPING, "sleeping"},
    {Status::STOPPING, "stopping"}, {Status::ERROR, "error"}};

} // namespace node

#endif