/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   extension_module.hpp
 * @brief  This is the header file for the new extension module.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-12
 * @copyright  2023 (license to be defined)
 */

#ifndef EXTENSION_MODULE_HPP_
#define EXTENSION_MODULE_HPP_

#include "extension.hpp"
#include "module.hpp"
#include <vector>

namespace modules {

/**
 * @brief The maximum number of extensions.
 * @details I can't think of any reason with the current design that
 * any more than 10 extensions would be needed, but this can
 * be changed if needed obviously. The only case where this
 * limit would be hit right now, would be on a configuration failure.
 */
inline constexpr int kMaxExtensionCount = 10;

class ExtensionModule : public Module {
public:
  ExtensionModule(data::SharedData &, cfg::Configuration &);
  ~ExtensionModule() override = default;

private:
  /**
   * @brief The action determines the targeted state of the extension.
   * @details This is how the extension module keeps track of what it is trying
   * to do with an extension.
   */
  enum class ExtAction;

  /**
   * @brief The container that holds the information about an extension and the
   * extension itself.
   *
   * @details Includes ExtState, the metadata, and running information to keep
   * track of what the module is trying to do to be able to compare that to
   * what the extension is actually doing.
   */
  struct ExtContainer;

  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const cmd::Command &) override;
  void updateLocalConfig() override;

  ext::ExtensionInterfaces extension_interfaces_;
  std::vector<ext::Extension> extensions_{};

  std::vector<cfg::ExtensionMetadata> extension_metadata_{};

  // configuration variables
  int status_polling_interval_ = 0;
  int max_restart_attempts_ = 0;
  int restart_delay_ = 0;
  int startup_timeout_ = 0;
};

} // namespace modules

#endif /* EXTENSION_MODULE_HPP_ */
