import file_gen_utils as utils
import json
import sys

INDENT = "  "
SECTION_TYPE_KEY = "SECTION_TYPE"
SECTION_ID_KEY = "ID"

CFG_NAMESPACE = "cfg"
CFG_ENUM_NAMESPACE = "gEnum"
CFG_SECTIONS_NAMESPACE = "sections"

RESERVED_KEYS = [SECTION_TYPE_KEY, SECTION_ID_KEY]

# Default Data
def defData(json_section_key, json_setting_key, name):
    return {"json_section_key": json_section_key, "json_setting_key": json_setting_key, "name": name}

class StructItem:
    def __init__(self, default_data):
        self.default = default_data
    
    def getDataMember(self):
        return f"{self.type} {self.member_name} = default;"

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

class StructItem:
    def __init__(self):
        self.type = None;
        self.member_name = None;
        self.default_value = None;
        self.validator = None;
    
    def setVals(self, cpp_type, member_name, default_value):
        self.type = cpp_type
        self.member_name = member_name
        self.default_value = default_value
        
    def setStringValidator(self, min, max, pattern):
        self.validator = "validator"
    
    def setNumValidator(self, min, max):
        self.validator = "validator"
        
    def setEnumValidator(self, enum_name):
        self.validator = "validator"
        
    def setBoolValidator(self):
        self.validator = "validator"
    
    def getDef(self):
        return f'{self.type} {self.member_name} = {self.default_value};'
    
class Section:
    def __init__(self, section_id):
        self.section_id = section_id
        self.items = []

    def addItem(self, struct_item):
        self.items.append(struct_item)

    def getStruct(self):
        strs = []
        for item in self.items:
            strs.append(item.getDef())
        return utils.struct(self.section_id, strs)

class ConfigGen:
    def __init__(self, meta: dict, out_dir: str):
        self.meta = meta
        self.out_dir = out_dir

        # section
        self.sec_name = None
        self.sec_contents = None
        self.sec_type = None
        self.sec_id = "NO_ID"
        
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
                self.sec_id = self.sec_contents[SECTION_ID_KEY]
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
        section = Section(self.sec_id)
        for key in self.sec_contents:
            if key in RESERVED_KEYS:
                continue
            
            item = StructItem()
            
            set_data = self.sec_contents[key]
            
            default_data = defData(self.sec_name, key, set_data["name"])
            #print(default_data)
            
            set_type = set_data["type"]
            
            set_default = set_data["default"]
            
            set_min = None
            set_max = None
            set_pattern = None
            
            cpp_type = ""
            cpp_member_name = key
            
            if set_type == "string":
                cpp_type = "std::string"
                set_default = f'"{set_default}"'
                set_min = set_data["min"]
                set_min = set_data["max"]
                set_pattern = set_data["pattern"]
                
            elif set_type == "bool":
                cpp_type = "bool"
                
            elif set_type == "int":
                cpp_type = "int"
                
                
            elif set_type == "float":
                cpp_type = "float"

            elif set_type == "enum":
                enum_name = set_data["enum_name"]
                enum_options = set_data["options"]
                cpp_type = enumType(enum_name)
                
                set_default = enumValue(enum_name, set_default)
                
                if enum_name not in self.defined_enum_ids:
                    enum = Enum(enum_name, enum_options)
                    self.enums.append(enum)
                    self.defined_enum_ids.append(enum_name)
            item.setVals(cpp_type, key, set_default)
            
            section.addItem(item)
            
        self.struct_items.append(section)
            
    def StructureFile(self):
        STRUCTURE_FILE_NAME = "configuration_structure"
        STRUCTURE_FILE_INCLUDES = ["<string>"]
        
        file = utils.headerFileHeader(STRUCTURE_FILE_NAME, STRUCTURE_FILE_INCLUDES)
        file += utils.enterNameSpace(CFG_NAMESPACE)
        
        # enums
        file += utils.enterNameSpace(CFG_ENUM_NAMESPACE) + "\n"
        for enum in self.enums:
            file += enum.getDefinition() + "\n\n"
        file += utils.exitNameSpace(CFG_ENUM_NAMESPACE)
        
        
        # sections
        file += utils.enterNameSpace(CFG_SECTIONS_NAMESPACE)
        for section in self.struct_items:
            file += section.getStruct();
        file += utils.exitNameSpace(CFG_SECTIONS_NAMESPACE)

        
        file += f'}} // namespace {CFG_NAMESPACE}\n'
        file += utils.headerFileFooter(STRUCTURE_FILE_NAME)
        
        f = open(self.out_dir + "/" + STRUCTURE_FILE_NAME + ".hpp", "w")
        f.write(file)
        
    def ValidatorsFile(self):
        pass

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