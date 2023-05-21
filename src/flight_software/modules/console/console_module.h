#ifndef CONSOLE_MODULE_H_
#define CONSOLE_MODULE_H_

#include <vector>

#include "curses_utilities.h"
#include "module.h"
#include "streams.h"

namespace modules {
class ConsoleModule : public Module {
 public:
  ConsoleModule(data::Streams &streams, cfg::Configuration &config);
  ~ConsoleModule() override;
  ConsoleModule(const ConsoleModule &) = delete;  // No copy constructor
  ConsoleModule &operator=(const ConsoleModule &) =
      delete;  // No copy assignment

 private:
  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const command::Command &command);

  ncurs::Environment ncurs_env_ = ncurs::Environment();
};

}  // namespace modules

#endif