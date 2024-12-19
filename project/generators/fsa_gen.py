import sys
import json
from cpp_gen_v2 import FileGenerator, Enum, Map, TextBlock, Function, Namespace, Vector, Templater

assert len(sys.argv) == 3, "usage: bit_test_gen.py <metadata_json> <output_hpp_file> <output_cpp_file>"

in_path = sys.argv[1]
input_json = json.load(open(in_path, "r"))
settings = input_json["settings"]

out_path = sys.argv[2]
out_cpp = FileGenerator("")
out_cpp.addIncludes('"agent_settings.hpp"')

cpp_namespace = Namespace("command_line_interface")
settings_map = Map("settings_map_", "std::string", "AgentSettings::Setting", "std::map", clang_format_off=True)

for setting_label in settings:
    setting_type = f'AgentSettings::SettingType::{settings[setting_label]["type"].upper()}'
    setting_name = settings[setting_label]["name"]
    setting_default = settings[setting_label]["default"]

    if setting_type == "AgentSettings::SettingType::STRING":
        setting_default = f'"{setting_default}"'
    elif setting_type == "AgentSettings::SettingType::BOOL":
        setting_default = "false" if setting_default == "false" else "true"

    settings_map.addPair(f'"{setting_label}"', f'AgentSettings::Setting{{"{setting_label}", {setting_type}, {setting_default}}}')

templater = Templater(out_path)
# print(settings_map.get())
templater.template("settings_map_", settings_map.get(1))

# cpp_namespace.addComponent(settings_map)

# out_cpp.addComponent(cpp_namespace)
# out_cpp.write(out_path)