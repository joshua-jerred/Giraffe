/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   cfg_extensions.cpp
 * @brief  Implements the extensions section of the configuration.
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-12
 * @copyright  2023 (license to be defined)
 */

#include "sections/cfg_extensions.hpp"
#include "validation.hpp"
#include <iostream>

namespace cfg {

Json ExtensionMetadata::getJson() const {
  return Json::object({
      {"name", name},
      {"enabled", enabled},
      {"type", gEnum::K_EXTENSION_TYPE_TO_STRING_MAP.at(type)},
      {"update_interval", update_interval},
      {"critical", critical},
      {"extra_args", extra_args},
  });
}

void ExtensionMetadata::setFromJson(const Json &j, data::LogStream &log,
                                    const std::string &ext_index) {

  const std::string section_name = "extensions." + ext_index;

  validation::setValidValue<std::string>(log, j, section_name, "name", name, 3,
                                         10, "");
  validation::setValidValue<bool>(log, j, section_name, "enabled", enabled, 0,
                                  0, "");
  validation::setValidEnum<gEnum::ExtensionType>(
      log, j, section_name, "type", type,
      gEnum::K_STRING_TO_EXTENSION_TYPE_MAP);
  validation::setValidValue<int>(log, j, section_name, "update_interval",
                                 update_interval, 5, 60000, "");
  validation::setValidValue<bool>(log, j, section_name, "critical", critical, 0,
                                  0, "");
  validation::setValidValue<std::string>(log, j, section_name, "extra_args",
                                         extra_args, 0, 100, "");
}

// Extensions (class) --------------------------

void Extensions::setFromJson(const Json &j) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);

  // Ensure that the extensions are an array.
  if (!j.is_array()) {
    error(DiagnosticId::CONFIG_extensionsNotArray);
    return;
  }

  // Iterate over the extensions.
  int num_attempted = 0;
  for (const Json &ext_meta_j : j) {
    if (!ext_meta_j.is_object()) {
      error(DiagnosticId::CONFIG_extensionNotObject,
            std::to_string(num_attempted++));
      continue;
    }

    // Create the extension metadata.
    ExtensionMetadata ext_meta;
    std::string index = std::to_string(num_attempted);
    ext_meta.setFromJson(ext_meta_j, streams_.log, index);

    // Check if the extension name is already in use.
    if (doesNameExist(ext_meta.name)) {
      error(DiagnosticId::CONFIG_extensionNameAlreadyExists,
            "on load " + ext_meta.name);
      continue;
    }

    // Add the extension metadata to the list.
    extensions_.push_back(ext_meta);
    num_attempted++;
  }
}

Json Extensions::getJson() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  Json j = Json::array();
  for (const ExtensionMetadata &ext_meta : extensions_) {
    j.push_back(ext_meta.getJson());
  }
  return j;
}

bool Extensions::addExtension(const ExtensionMetadata &metadata) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  if (doesNameExist(metadata.name)) {
    error(DiagnosticId::CONFIG_extensionNameAlreadyExists,
          "on add " + metadata.name);
    return false;
  }

  extensions_.push_back(metadata);
  return true;
}

bool Extensions::removeExtension(const std::string &name) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  for (auto it = extensions_.begin(); it != extensions_.end(); it++) {
    if (it->name == name) {
      extensions_.erase(it);
      return true;
    }
  }
  error(DiagnosticId::CONFIG_extensionNameDoesNotExist, "on remove " + name);
  return false;
}

bool Extensions::updateExtension(const std::string &name,
                                 const ExtensionMetadata &metadata) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  for (auto it = extensions_.begin(); it != extensions_.end(); it++) {
    if (it->name == name) {
      *it = metadata;
      return true;
    }
  }
  error(DiagnosticId::CONFIG_extensionNameDoesNotExist, "on update " + name);
  return false;
}

void Extensions::toggleExtension(const std::string &name, bool enabled) {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  for (auto it = extensions_.begin(); it != extensions_.end(); it++) {
    if (it->name == name) {
      it->enabled = enabled;
      return;
    }
  }

  error(DiagnosticId::CONFIG_extensionNameDoesNotExist, "on toggle " + name);
}

std::vector<ExtensionMetadata> Extensions::getExtensions() const {
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return extensions_;
}

bool Extensions::doesNameExist(const std::string &name) const {
  for (const ExtensionMetadata &ext_meta : extensions_) {
    if (ext_meta.name == name) {
      return true;
    }
  }
  return false;
}

} // namespace cfg