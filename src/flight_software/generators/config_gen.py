import json
import sys

INDENT = "  "
SECTION_TYPE_KEY = "SECTION_TYPE"
SECTION_ID_KEY = "ID"
CFG_NAMESPACE = "cfg"
CFG_ENUM_NAMESPACE = "g_enum"

RESERVED_KEYS = [SECTION_TYPE_KEY, SECTION_ID_KEY]

def headerFileHeader(file_name: str): # file_name ex: structure - turns into structure.hpp
    return """// * * * GENERATED WITH config_gen.py * * *

#ifndef {1}_HPP_
#define {1}_HPP_
""".format(file_name, file_name.upper())

def headerFileFooter(file_name: str):
    return """
#endif // {0}_HPP_

// * * * GENERATED WITH config_gen.py * * *
""".format(file_name.upper())

# Default Data
def defData(json_section_key, json_setting_key, name):
    return {"json_section_key": json_section_key, "json_setting_key": json_setting_key, "name": name}

class StructItem:
    def __init__(self, default_data):
        self.default = default_data
    
    def getDataMember(self):
        return f"{self.type} {self.member_name}"
    
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
            cases += f'{INDENT}{INDENT}case {self.name_with_ns}::{option}: "{option.lower()}";\n'
        
        close_line = f'{INDENT}{INDENT}default: throw std::invalid_argument("String Not Implemented in {self.name}");\n{INDENT}}}\n}}'
        return open_line + cases + close_line

    def getJsonKeyToEnum(self):
        open_line = f'static std::unordered_map<std::string, {self.name_with_ns}> const table = {{\n'
        cases = ""
        
        first = True
        for option in self.options:
            if not first:
                cases += ",\n"
            cases += f'{INDENT}{{"{option.lower()}", {self.name_with_ns}::{option}}}'
            first = False
        
        close_line = "\n};"
        return open_line + cases + close_line

class ConfigGen:
    def __init__(self, meta: dict, out_dir: str):
        self.meta = meta
        self.out_dir = out_dir

        # section
        self.sec_name = None
        self.sec_contents = None
        self.sec_type = None
        
        self.struct_items = []
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

            if SECTION_TYPE_KEY in self.sec_contents:
                self.sec_type = self.sec_contents[SECTION_TYPE_KEY]
            else:
                self.error("No Section Type")
                continue

            if self.sec_type == "struct":
                self.parseStructSection()
            elif self.sec_type == "list":
                self.error("list not implemented")
            else:
                self.error("Invalid Section Type")
            
            # temporary
            if i == 1:
                print(self.StructureFile())
                return 
            i += 1

    def parseStructSection(self):
        for key in self.sec_contents:
            if key in RESERVED_KEYS:
                continue
            
            set_data = self.sec_contents[key]
            
            default_data = defData(self.sec_name, key, set_data["name"])
            #print(default_data)
            
            set_type = set_data["type"]
            
            set_default = set_data["default"]
            
            set_min = None
            set_max = None
            set_pattern = None
            
            cpp_type = ""
            cpp_member_name = ""
            
            if set_type == "string":
                cpp_type = "std::string"
                set_min = set_data["min"]
                set_min = set_data["max"]
                set_pattern = set_data["pattern"]
            elif set_type == "bool":
                cpp_type = "bool"
            elif set_type == "int":
                cpp_type == "int"
            elif set_type == "float":
                cpp_type == "float"
            elif set_type == "enum":
                enum_name = set_data["enum_name"]
                enum_options = set_data["options"]
                enum = Enum(enum_name, enum_options)
                self.enums.append(enum)
                self.defined_enum_ids.append(enum_name)

            print(f"{cpp_type}")
            
    def StructureFile(self):
        file = headerFileHeader("structure")
        file += f"\nnamespace {CFG_NAMESPACE} {{\nnamespace {CFG_ENUM_NAMESPACE} {{\n\n"
        for enum in self.enums:
            file += enum.getDefinition() + "\n\n"
        file += f'}} // namespace {CFG_ENUM_NAMESPACE}\n'
        
        file += f'}} // namespace {CFG_NAMESPACE}\n'
        file += headerFileFooter("structure")
        return file
        
        



if __name__ == "__main__":
    print("\nGenerating Configuration Code")
    
    if len(sys.argv) != 3:
        print("ERROR: Invalid Args!\n")
        sys.exit(-1)
    
    meta_path = sys.argv[1]
    config_dir = sys.argv[2]
    meta = json.load(open(meta_path))

    cfg_gen = ConfigGen(meta, config_dir)
    cfg_gen.generate()
    
    print("Done.\n")