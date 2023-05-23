#ifndef CONSOLE_MODULE_H_
#define CONSOLE_MODULE_H_

#include <vector>

#include "curses_utilities.h"
#include "module.h"

namespace modules {
class ConsoleModule : public Module {
 public:
  ConsoleModule(data::SharedData &, cfg::Configuration &);
  ~ConsoleModule() override;
  ConsoleModule(const ConsoleModule &) = delete;  // No copy constructor
  ConsoleModule &operator=(const ConsoleModule &) =
      delete;  // No copy assignment

 private:
  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const command::Command &);

  ncurs::Environment ncurs_env_ = ncurs::Environment();

  std::vector<std::string> status();

  std::vector<std::string> configSection(cfg::Section);
};

}  // namespace modules

#endif