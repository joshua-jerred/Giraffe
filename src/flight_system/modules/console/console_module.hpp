#ifndef CONSOLE_MODULE_HPP_
#define CONSOLE_MODULE_HPP_

#include <vector>

#include "console_pages.hpp"
#include "curses_utilities.hpp"
#include "module.hpp"

namespace modules {
/**
 * @brief The console module is a module that provides a console interface for
 * the flight system.
 */
class ConsoleModule : public Module {
public:
  /**
   * @brief Construct the console module.
   * @param shared_data The globally shared data.
   * @param config The global configuration.
   */
  ConsoleModule(data::SharedData &, cfg::Configuration &);
  ~ConsoleModule() override;
  ConsoleModule(const ConsoleModule &) = delete; // No copy constructor
  ConsoleModule &
  operator=(const ConsoleModule &) = delete; // No copy assignment

private:
  /**
   * @brief The startup function that is called by the base class.
   */
  void startup() override;

  /**
   * @brief The loop function that is called by the base class.
   */
  void loop() override;

  /**
   * @brief The shutdown function that is called by the base class.
   */
  void shutdown() override;

  /**
   * @brief The process command function that is called by the base class.
   * Processes commands sent to the module.
   */
  void processCommand(const cmd::Command &);

  console_pages::Pages pages_;
  ncurs::Environment ncurs_env_;

  std::vector<std::string> status();

  std::vector<std::string> configSection();
};

} // namespace modules

#endif