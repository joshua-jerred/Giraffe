# absolute spaghetti monster below, you've been warned.

import file_gen_utils as utils
import config_gen_components
import json
import sys
import os

INDENT = "  "
SECTION_TYPE_KEY = "SECTION_TYPE"
SECTION_ID_KEY = "ID"
LIST_STRUCT_NAME_KEY = "LIST_STRUCT_NAME"

CFG_NAMESPACE = "cfg"
CFG_ENUM_NAMESPACE = "gEnum"
CFG_JSON_NAMESPACE = "json_data"

RESERVED_KEYS = [SECTION_TYPE_KEY, SECTION_ID_KEY, LIST_STRUCT_NAME_KEY]

STRUCTURE_FILE_NAME = "configuration"
JSON_FILE_NAME = "configuration"

# Default Data
def defData(json_section_key, json_setting_key, name):
    return {"json_section_key": json_section_key, "json_setting_key": json_setting_key, "name": name}

def structType(name):
    name = name.replace("_", " ").title().replace(" ", "")
    return f'{CFG_NAMESPACE}::{name}'

def enumType(name):
    name = name.replace("_", " ").title().replace(" ", "")
    return f'{CFG_NAMESPACE}::{CFG_ENUM_NAMESPACE}::{name}'

def enumValue(type_name, value):
    return f'{enumType(type_name)}::{value.upper()}'

class Enum:
    def __init__(self, enum_name: str, enum_options: list):
        # Remove underscores from name and capitalize
        self.name = enum_name.replace("_", " ").title().replace(" ", "")
        self.name_with_ns = f'{CFG_NAMESPACE}::{CFG_ENUM_NAMESPACE}::{self.name}'
        # Upper all options
        self.options = [option.upper() for option in enum_options]
    
    def getDefinition(self):
        return f"enum class {self.name} {{\n{self.getOptions()}\n}};"
    
    def getOptions(self):
        options = ""
        first = True
        for option in self.options:
            if not first:
                options += ",\n"
            options += f"{INDENT}{option}"
            first = False
        return options
    
    def getEnumToJsonKey(self):
        open_line = f"constexpr const char* {self.name}ToKey({self.name_with_ns} val) throw() {{\n{INDENT}switch (val) {{\n"
        cases = ""
        
        for option in self.options:
            cases += f'{INDENT}{INDENT}case {self.name_with_ns}::{option}: return "{option.lower()}";\n'
        
        unreachable = f"{INDENT}__builtin_unreachable();"# f'{INDENT}{INDENT}default: throw std::invalid_argument("String Not Implemented in {self.name}");\n'
        
        close = f'{INDENT}}}\n{unreachable}\n}}\n'
        return open_line + cases + close

    def getJsonKeyToEnum(self):
        open_line = f'static std::unordered_map<std::string, {self.name_with_ns}> const KeyTo{self.name} = {{\n'
        cases = ""
        
        first = True
        for option in self.options:
            if not first:
                cases += ",\n"
            cases += f'{INDENT}{{"{option.lower()}", {self.name_with_ns}::{option}}}'
            first = False
        
        close_line = "\n};\n"
        return open_line + cases + close_line

class SectionItem:
    def __init__(self):
        self.type = None;
        self.member_name = None;
        self.default_value = None;
        self.validator = None;
    
    def setVals(self, cpp_type, member_name, default_value, json_type, section_name, validation):
        self.type = cpp_type
        self.member_name = member_name
        self.default_value = default_value
        self.json_type = json_type
        self.section_name = section_name
        self.validation = validation

        name_parts = self.member_name.split("_");
        self.name_capitalized = ""
        for i in name_parts:
            self.name_capitalized += i.title()
            
    def getJsonKey(self):
        return self.member_name
    
    def getJsonValueFromStruct(self):
        member = self.member_name + "_"
        
        if self.json_type == "enum":
            return f'{self.type}ToKey({member})'
        return member
        
    def setValidator(self):
        nl = "\n" + INDENT * 4 # newline with indent
        ret_val = ""
        
        # enum types
        if (self.json_type == "enum"):
            ret_val += f'{INDENT}validation::setValidEnum<{self.type}>({nl}'
            ret_val += f'streams_.log,{nl}json_data,{nl}'
            ret_val += f'"{self.section_name}",'
            ret_val += f'{nl}"{self.member_name}",'
            ret_val += f'{nl}{self.member_name}_,'
            
            map_name = "cfg::gEnum::KeyTo" + self.type.split("::")[-1]
            ret_val += f'{nl}{map_name}'
        
        else: # all types
            ret_val += f'{INDENT}validation::setValidValue<{self.type}>({nl}'
            ret_val += f'streams_.log,{nl}json_data,{nl}'
            ret_val += f'"{self.section_name}",'
            ret_val += f'{nl}"{self.member_name}",'
            ret_val += f'{nl}{self.member_name}_,'
            ret_val += f'{nl}{self.validation["min"]},'
            ret_val += f'{nl}{self.validation["max"]},'
            ret_val += f'{nl}"{self.validation["pattern"]}"'
        
        ret_val += f'\n{INDENT});'
        return ret_val

    def getDef(self):
        return f'{self.type} {self.member_name}_ = {self.default_value};'
    
    def getGetterDec(self):
        return f'{self.type} get{self.name_capitalized}() const;'
    
    def getSetterDec(self):
        return f'void set{self.name_capitalized}({self.type});'
    
    def getGetterDef(self, class_name):
        return f'''{self.type} {class_name}::get{self.name_capitalized}() const {{
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  return {self.member_name}_;
}}\n'''

    def getSetterDef(self, class_name):
        return f'''void {class_name}::set{self.name_capitalized}({self.type} val) {{
  const std::lock_guard<std::mutex> lock(cfg_lock_);
  {self.member_name}_ = val;
}}\n'''


class Section:
    def __init__(self, section_id, is_list, list_struct_name = None):
        self.section_member = section_id # The member name in the main struct
        self.is_list = is_list
        if is_list:
            self.list_name = section_id
            self.section_member = list_struct_name
        sec_id = section_id.split("_")
        self.section_id = ""
        for i in sec_id:
            self.section_id += i.title()
        self.items = []

    def addItem(self, struct_item):
        self.items.append(struct_item)

    def getHeaderString(self):        
        ret_str = f"class {self.section_id} : public cfg::CfgSection {{\n"
        ret_str += "public:\n"
        ret_str += f"{INDENT}{self.section_id}(data::Streams &streams): cfg::CfgSection(streams){{}}\n\n"
        ret_str += self._public_members()
        ret_str += "\nprivate:\n"
        ret_str += self._private_members()
        ret_str += "};\n\n"
        return ret_str
    
    def getCppStringGetter(self):
        ret_str = ""
        for item in self.items:
            ret_str += item.getGetterDef(f'{CFG_NAMESPACE}::{self.section_id}') + "\n"
        return ret_str

    def getCppStringSetter(self):
        ret_str = ""
        for item in self.items:
            ret_str += item.getSetterDef(f'{CFG_NAMESPACE}::{self.section_id}') + "\n"
        return ret_str
    
    def getStructDecString(self):
        if not self.is_list:
            return f"{INDENT}{CFG_NAMESPACE}::{self.section_id} {self.section_member};" # = {CFG_NAMESPACE}::{self.section_id}(streams_);"
        else:
            vector_type = structType(self.list_name)
            return f"{INDENT}std::vector<{vector_type}> {self.section_member} = {{}};"

    def _public_members(self):
        public = ""
        for item in self.items:
            public += f'{INDENT}{item.getGetterDec()}\n'
        public += "\n"
        for item in self.items:
            public += f'{INDENT}{item.getSetterDec()}\n'
        
        public += "\n"
        
        public += f"{INDENT}void setFromJson(const json&);\n"
        public += f"{INDENT}json getJson() const;\n"
        return public

    def _private_members(self):
        private = ""
        for item in self.items:
            private += f'{INDENT}{item.getDef()}\n'
        
        #private += "\n"
        
        #private += f"{INDENT}mutable std::mutex cfg_lock_ = std::mutex();\n{INDENT}data::Streams &streams_;\n"
        return private

    def parseJsonToStructDef(self):
        contents = "{\n"
        contents += f'{INDENT}const std::lock_guard<std::mutex> lock(cfg_lock_);\n'

        for item in self.items:
            contents += f'{item.setValidator()}\n'
        
        contents += "}\n"
        return f'void {CFG_NAMESPACE}::{self.section_id}::setFromJson(const json &json_data) {contents}\n'
    
    def parseStructToJsonDef(self):
        contents = "{\n"
        contents += f'{INDENT}const std::lock_guard<std::mutex> lock(cfg_lock_);\n'
        
        contents += f'{INDENT}return json({{\n'
        
        first = True
        for item in self.items:
            if not first:
                contents += ",\n"
            contents += f'{INDENT*2}{{"{item.getJsonKey()}", {item.getJsonValueFromStruct()}}}'
            first = False
            
        contents += f"\n{INDENT}}});\n"
        
        contents += "}\n"
        return f'json {CFG_NAMESPACE}::{self.section_id}::getJson() const {contents}'

class ConfigGen:
    def __init__(self, meta: dict, out_dir: str):
        self.meta = meta
        self.out_dir = out_dir

        # section
        self.sec_name = None
        self.sec_contents = None
        self.sec_type = None
        self.sec_id = "NO_ID"
        
        self.sections = []
        self.enums = []
        self.defined_enum_ids = [] # list of json ids that are already enums

    def error(self, info):
        print(f"  *ERR*  {self.sec_name}: {info}")

    def generate(self):
        i = 0 # temporary
        
        for key in self.meta:
            self.sec_name = key
            print(self.sec_name)
            self.sec_contents = self.meta[key]

            if "IGNORE" in self.sec_contents: # ignore this section
                continue

            if SECTION_TYPE_KEY in self.sec_contents:
                self.sec_type = self.sec_contents[SECTION_TYPE_KEY]
                self.sec_id = key
            else:
                self.error("No Section Type")
                continue

            if self.sec_type == "struct":
                self.parseStructSection(False)
            elif self.sec_type == "list":
                list_name = self.sec_contents["LIST_STRUCT_NAME"]
                self.parseStructSection(True, list_name)
            else:
                self.error("Invalid Section Type")
            
            self.StructureHeader()
            self.StructureCpp()

    def parseStructSection(self, is_list, list_name=""):
        section = Section(self.sec_id, is_list, list_name)
        for key in self.sec_contents:
            if key in RESERVED_KEYS:
                continue
            
            item = SectionItem()
            
            set_data = self.sec_contents[key]
            set_type = set_data["type"]
            set_default = set_data["default"]
            
            set_min = 0
            set_max = 0
            set_pattern = ""
            
            cpp_type = ""
            
            if set_type == "string":
                cpp_type = "std::string"
                set_default = f'"{set_default}"'
                set_min = set_data["min"]
                set_max = set_data["max"]
                set_pattern = set_data["pattern"]
                
            elif set_type == "bool":
                cpp_type = "bool"
                set_default = "true" if set_default else "false"
                
            elif set_type == "int":
                cpp_type = "int"
                set_min = set_data["min"]
                set_max = set_data["max"]
                
            elif set_type == "float":
                cpp_type = "float"
                set_min = set_data["min"]
                set_max = set_data["max"]

            elif set_type == "enum":
                enum_name = set_data["enum_name"]
                enum_options = set_data["options"]
                cpp_type = enumType(enum_name)
                
                set_default = enumValue(enum_name, set_default)
                
                if enum_name not in self.defined_enum_ids:
                    enum = Enum(enum_name, enum_options)
                    self.enums.append(enum)
                    self.defined_enum_ids.append(enum_name)
            validation = {
                "min":set_min,
                "max":set_max,
                "pattern":set_pattern
            }
            item.setVals(cpp_type, key, set_default, set_type, self.sec_name, validation)
            
            section.addItem(item)
            
        self.sections.append(section)
            
    def StructureHeader(self):
        STRUCTURE_FILE_INCLUDES = ["<string>", "<fstream>", "<mutex>", "<unordered_map>", "<vector>", "<nlohmann/json.hpp>", '"shared_data.hpp"']
        
        file = utils.headerFileHeader(STRUCTURE_FILE_NAME, STRUCTURE_FILE_INCLUDES)
        file += "using json = nlohmann::ordered_json;\n\n"
        file += utils.enterNameSpace(CFG_NAMESPACE)
        
        # enums
        file += utils.enterNameSpace(CFG_ENUM_NAMESPACE) + "\n"
        for enum in self.enums:
            file += enum.getDefinition() + "\n"
            file += enum.getJsonKeyToEnum()
            file += enum.getEnumToJsonKey()
            file += "\n"

        file += utils.exitNameSpace(CFG_ENUM_NAMESPACE)
        
        # sections
        file += config_gen_components.base_class
        
        for section in self.sections:
            file += section.getHeaderString();
        
        
        # main config struct
        general_config_member_names = []
        general_class_member_decs = []
        for section in self.sections:
            general_class_member_decs.append(f"{section.getStructDecString().strip()}")
            if section.is_list:
                continue
            general_config_member_names.append(section.section_member)
            
        file += config_gen_components.getConfigurationClass(general_config_member_names, general_class_member_decs)
        

        file += f'}} // namespace {CFG_NAMESPACE}\n'
        file += utils.headerFileFooter(STRUCTURE_FILE_NAME)
        f = open(self.out_dir + "/" + STRUCTURE_FILE_NAME + ".hpp", "w")
        f.write(file)

    def StructureCpp(self):
        STRUCTURE_FILE_INCLUDES = ["<filesystem>", f'"{STRUCTURE_FILE_NAME}.hpp"', '"validation.hpp"']
        file = utils.cppFileHeader(STRUCTURE_FILE_NAME, STRUCTURE_FILE_INCLUDES)
        file += "using json = nlohmann::ordered_json;\n\n"
        
        section_member_names = []
        # getters/setters
        for section in self.sections:
            section_member_names.append(section.section_member)
            file += section.getCppStringGetter();
            file += section.getCppStringSetter();
            file += section.parseJsonToStructDef();
            file += section.parseStructToJsonDef();
        
        file += config_gen_components.getConfigurationSaveAndLoad(section_member_names)

        file += utils.cppFileFooter(STRUCTURE_FILE_NAME)

        f = open(self.out_dir + "/" + STRUCTURE_FILE_NAME + ".cpp", "w")
        f.write(file)

if __name__ == "__main__":
    print("\nGenerating Configuration Code")
    
    IN_FILE = os.getenv('CONFIG_GEN_IN_FILE_PATH')
    if (IN_FILE == None):
        print("Error: CONFIG_GEN_IN_FILE_PATH environment variable not set. Set from vscode tasks.json")
        sys.exit(1)
    else:
        print("IN_FILE: " + IN_FILE)

    OUT_DIR = os.getenv('CONFIG_GEN_OUT_FILE_PATH')
    if (OUT_DIR == None):
        print("Error: CONFIG_GEN_OUT_FILE_PATH environment variable not set. Set from vscode tasks.json")
        sys.exit(1)
    else:
        print("OUT_FILE: " + OUT_DIR)
    
    meta_path = IN_FILE
    config_dir = OUT_DIR
    meta = json.load(open(meta_path))

    cfg_gen = ConfigGen(meta, config_dir)
    cfg_gen.generate()
    
    print("Done.\n")