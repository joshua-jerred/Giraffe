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

#include <optional>
#include <vector>

#include <BoosterSeat/timer.hpp>

#include "extension_base.hpp"
#include "module.hpp"

namespace modules {

/**
 * @brief The maximum number of extensions.
 * @details I can't think of any reason with the current design that
 * any more than 10 extensions would be needed, but this can
 * be changed if needed obviously. The only case where this
 * limit would be hit right now, would be on a configuration failure.
 */
inline constexpr int K_MAX_EXTENSION_COUNT = 10;

class ExtensionModule : public Module {
public:
  ExtensionModule(data::SharedData &, cfg::Configuration &);
  ~ExtensionModule() override = default;

#if GFS_UNIT_TEST_BUILD == 1

  /// @brief Not super happy about this, there are many better ways. But for
  /// now, I'm more concerned about extension testing.
  std::optional<extension::Extension *> getExtension(const std::string &name) {
    for (auto &ext : extensions_) {
      if (ext.metadata.name == name) {
        return ext.extension.get();
      }
    }
    return std::nullopt;
  }
#endif

private:
  /**
   * @brief The action determines the targeted state of the extension.
   * @details This is how the extension module keeps track of what it is trying
   * to do with an extension.
   *
   * @see extension_module_actions.puml
   */
  enum class ExtAction {
    UNKNOWN,
    DISABLE,
    START,
    RUN,
    STOP,
    RESTART,
    ERROR_START,
    ERROR_STOP,
    ERROR_RESTART,
    ERROR_DISABLE,
  };

  /// @brief Converts the state machine action to a string.
  static std::string actionToString(ExtAction action) {
    switch (action) {
    case ExtAction::UNKNOWN:
      return "UNKNOWN";
    case ExtAction::DISABLE:
      return "DISABLE";
    case ExtAction::START:
      return "START";
    case ExtAction::RUN:
      return "RUN";
    case ExtAction::STOP:
      return "STOP";
    case ExtAction::RESTART:
      return "RESTART";
    case ExtAction::ERROR_START:
      return "ERROR_START";
    case ExtAction::ERROR_STOP:
      return "ERROR_STOP";
    case ExtAction::ERROR_RESTART:
      return "ERROR_RESTART";
    case ExtAction::ERROR_DISABLE:
      return "ERROR DISABLE";
    default:
      return "UNKNOWN";
    }
  }

  /**
   * @brief The container that holds the information about an extension and the
   * extension itself.
   *
   * @details Includes ExtState, the metadata, and running information to keep
   * track of what the module is trying to do to be able to compare that to
   * what the extension is actually doing.
   */
  struct ExtContainer {
    ExtContainer(std::shared_ptr<extension::Extension> ext,
                 cfg::ExtensionMetadata meta, uint32_t startup_timeout_ms,
                 uint32_t restart_delay_ms)
        : extension(ext), metadata(meta),
          startup_shutdown_timer(startup_timeout_ms),
          restart_timer(restart_delay_ms) {
    }

    std::shared_ptr<extension::Extension> extension;
    cfg::ExtensionMetadata metadata;

    ExtAction action = ExtAction::UNKNOWN;
    int startup_attempts = 0;
    int restart_attempts = 0;

    /**
     * @brief The timer used to determine if the extension has started up
     * in the allotted time.
     */
    bst::Timer startup_shutdown_timer;
    bst::Timer restart_timer;
  };

  void startup() override;
  void loop() override;
  void shutdown() override;
  void processCommand(const cmd::Command &) override;
  void updateLocalConfig() override;

  void addExtension(const cfg::ExtensionMetadata &extension_metadata);
  std::optional<ExtContainer> createExtension(const cfg::ExtensionMetadata &);

  void addPreConfiguredExtension(const std::string &ext_option);

  // State Machine Functions for each action
  void stateMachine(ExtContainer &);
  // not so thread safe...
  void unknownState(ExtContainer &);
  void disableState(ExtContainer &);
  void startState(ExtContainer &);
  void runState(ExtContainer &);
  void stopState(ExtContainer &);
  void restartState(ExtContainer &);
  void errorStartState(ExtContainer &);
  void errorRestartState(ExtContainer &);
  void errorDisableState(ExtContainer &);

  extension::ExtensionResources extension_resources_;
  std::vector<ExtContainer> extensions_{};

  std::vector<cfg::ExtensionMetadata> extension_metadata_{};

  bst::Timer status_polling_timer_{};

  data::blocks::ExtensionModuleStats stats_{};

  // configuration variables
  int status_polling_interval_ = 0;
  int max_restart_attempts_ = 0;
  int restart_delay_ = 0;
  int startup_timeout_ = 0;
  int max_startup_attempts_ = 0;
};

} // namespace modules

#endif /* EXTENSION_MODULE_HPP_ */
