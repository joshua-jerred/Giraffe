# Configuration Class

IND = "  " # indent

# more spaghetification
base_class = "" 


"""class CfgSection {
 public:
  CfgSection(data::Streams &streams): streams_(streams){}
  
 protected:
  mutable std::mutex cfg_lock_ = std::mutex();
  data::Streams &streams_;
};\n
"""

def getConfigurationClass(initializers, private_members):
    init_part = ""
    
    for initializer in initializers:
        init_part += f'{IND*2}{initializer}(streams),'
        
    members = ""
    for p in private_members:
        members += f'{IND}{p}\n'
        
    return """class Configuration {{
 public:
  Configuration(data::Streams &streams):
{0}
    streams_(streams){{}}
    
    void getAllJson(json &all_data) const;
    
    void save(std::string file_path = "");
    void load(std::string file_path = "");

{1} 
 private:
  void error(data::LogId error_code, std::string info = "") {{
      streams_.log.error(node::Identification::CONFIGURATION, error_code, info);
  }}
 
  data::Streams &streams_;
  std::mutex file_lock_ = std::mutex();
}};\n
""".format(init_part, members)


def getConfigurationSaveAndLoad(members):
    json_initializers = ""
    first = True
    for member in members:
        if not first:
            json_initializers += ","
        else:
            first = False
        json_initializers += f'{IND*2}{{"{member}", {member}.getJson()}}\n'
    
    json_loaders = ""
    for member in members:
        json_loaders += f'{IND}if (sectionExists(parsed, "{member}")) {{\n'
        json_loaders += f'{IND*2}{member}.setFromJson(parsed["{member}"]);'
        json_loaders += f'\n{IND}}} else {{'
        json_loaders += f'\n{IND*2}error(data::LogId::CONFIG_failedToLoadSectionNotFound, "{member}");'
        json_loaders += f'\n{IND}}}\n\n'
    return """
void cfg::Configuration::getAllJson(json &all_data) const {{
  all_data = {{
{0}  }};
}}
  
void cfg::Configuration::save(std::string file_path) {{
  const std::lock_guard<std::mutex> lock(file_lock_);
  
  std::ofstream out(file_path);
  
  if (out.fail()) {{
    error(data::LogId::CONFIG_failedToSaveToPath, file_path);
    return;
  }}
  
  json config_json;
  getAllJson(config_json);

  constexpr int json_indent = 2;
  std::string data = config_json.dump(json_indent);
  out << data;
}}

inline bool sectionExists(const json &all_data, const std::string &section_key) {{
  return all_data.contains(section_key);
}}

void cfg::Configuration::load(std::string file_path) {{
  const std::lock_guard<std::mutex> lock(file_lock_);

  if (!std::filesystem::exists(file_path)) {{
    error(data::LogId::CONFIG_configFileDoesNotExist, file_path);
    return; 
  }}

  std::ifstream in(file_path);
  
  if (in.fail()) {{
    error(data::LogId::CONFIG_failedToOpenConfig, file_path);
    return;
  }}
  
  json parsed;
  try {{
    parsed = json::parse(in);
  }} catch (json::parse_error &e) {{
    return;
  }}
  
{1}}}

""".format(json_initializers, json_loaders)