/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   cfg_extensions.hpp
 * @brief  The header file for the extensions section of the configuration.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-12
 * @copyright  2023 (license to be defined)
 */

#ifndef CFG_EXTENSIONS_HPP_
#define CFG_EXTENSIONS_HPP_

#include "configuration_enums.hpp"
#include "sections/cfg_section.hpp"

namespace cfg {
/**
 * @brief The metadata for an extension.
 */
struct ExtensionMetadata {

  /**
   * @brief The name of the extension, must be unique, used to identify the
   * extension.
   */
  std::string name = "";

  /**
   * @brief Whether or not the extension is enabled.
   */
  bool enabled = false;

  /**
   * @brief The type of extension.
   * @see ExtensionType
   */
  gEnum::ExtensionType type = static_cast<gEnum::ExtensionType>(0);

  /**
   * @brief The update interval for the extension in milliseconds.
   */
  int update_interval = 0;

  /**
   * @brief Whether or not the extension is critical to the operation of the
   * system.
   */
  bool critical = false;

  // extra arguments
  std::string extra_args = "";

  json getJson() const;

  void setFromJson(const json &j, data::LogStream &log,
                   const std::string &ext_index);
};

class Extensions : public cfg::CfgSection {
public:
  Extensions(data::Streams &streams) : cfg::CfgSection(streams) {
  }

  void setFromJson(const json &j);
  json getJson() const;

  /**
   * @brief Add an extension to the configuration.
   *
   * @warning This does not check for validity!
   *
   * @param metadata The metadata for the extension to add.
   */
  void addExtension(const ExtensionMetadata &metadata);

  /**
   * @brief Remove an extension from the configuration by name.
   *
   * @details If the extension does not exist, nothing changes
   * but an error is logged.
   *
   * @param name The name of the extension to remove.
   */
  void removeExtension(const std::string &name);

  /**
   * @brief Enable or disable an extension by name.
   *
   * @details If an extension with the given name does not exist,
   * nothing changes but an error is logged. If the extension is
   * already in the state requested, nothing happens.
   *
   * @param name The name of the extension to toggle.
   */
  void toggleExtension(const std::string &name, bool enabled);

  /**
   * @brief Returns a copy of the extension metadata configuration vector.
   * @return std::vector<ExtensionMetadata> All extension metadata loaded.
   */
  std::vector<ExtensionMetadata> getExtensions() const;

private:
  /**
   * @brief Checks if an extension with the given name exists.
   *
   * @warning This function does not lock the cfg_lock_ mutex.
   *
   * @param name The name of the extension to check for.
   * @return true If the extension exists.
   * @return false If the extension does not exist.
   */
  bool doesNameExist(const std::string &name) const;

  std::vector<ExtensionMetadata> extensions_{};
};
} // namespace cfg

#endif /* CFG_EXTENSIONS_HPP_ */