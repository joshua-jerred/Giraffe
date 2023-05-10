void Configuration::loadFromFile(std::string file_path, bool load_defaults_on_failure) {
  config_lock_.lock();

  if (!std::filesystem::exists(file_path)) {
    if (load_defaults_on_failure) loadDefaultConfiguration();
    config_lock_.unlock();
    throw ConfigurationError("File does not exist.");
  }

  YAML::Node config;
  try {
    config = YAML::LoadFile(file_path);
  } catch (std::runtime_error &e) {
    throw ConfigurationError(e.what());
  }

  if (config.IsNull()) {
    if (load_defaults_on_failure) loadDefaultConfiguration();
    config_lock_.unlock();
    throw ConfigurationError("File was empty.");
  } else if (!config.IsMap()) {
    throw ConfigurationError("Configuration file is not a map.");
  }

  config_lock_.unlock();
};

void Configuration::saveToFile(std::string file_path) {
  config_lock_.lock();
  YAML::Node out_contents;

  yamlAddGeneral(out_contents, cfg_general_);

  std::ofstream fout(file_path);
  fout << out_contents;
  fout.close();

  config_lock_.unlock();
}