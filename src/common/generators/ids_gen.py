import sys
import json5 as json
import os

import cpp_gen as cpp

INDENT = "  "

IN_FILE = os.getenv('ID_GEN_IN_FILE_PATH')
if (IN_FILE == None):
    print("Error: ID_GEN_IN_FILE_PATH environment variable not set. Set from vscode tasks.json")
    sys.exit(1)
else:
    print("IN_FILE: " + IN_FILE)

OUT_FILE = os.getenv('ID_GEN_OUT_FILE_PATH')
if (OUT_FILE == None):
    print("Error: ID_GEN_OUT_FILE_PATH environment variable not set. Set from vscode tasks.json")
    sys.exit(1)
else:
    print("OUT_FILE: " + OUT_FILE)

STRING_MAP_OUT = os.getenv('STRING_MAP_OUT_FILE_PATH')
print("STRING_MAP_OUT: ", STRING_MAP_OUT)

PRINT_OUTPUT = True if os.getenv('PRINT_OUTPUT') == "true" else False

class IdGenerator:
    def __init__(self, in_file, out_file) -> None:
        self.enum = None
        
        self.generate_string_map = False
        self.string_map_name = ""
        
        self.in_path = in_file
        self.out_path = out_file
        self.data = None
        
        self.namespaced = False
        self.namespace = ""
        self.enum_name = ""
        
        self.valued_enum = False # assume enum is not valued
        self.enum_type = None # type (ex. int, uint8_t, etc.)
        
        self.__loadData()
        self.__parse()
        self.__write()

    def __loadData(self):
        f = open(self.in_path, "r")
        self.data = json.load(f)
        
        try:
            self.namespace = self.data["FILE_META"]["namespace"]
            self.namespaced = True
        except KeyError:
            self.namespaced = False
            
        if "generate_string_map" in self.data["FILE_META"]:
            self.generate_string_map = True
            self.string_map_name = self.data["FILE_META"]["generate_string_map"]
            
        self.enum_name = self.data["FILE_META"]["enum_name"]
        
        if "enum_type" in self.data["FILE_META"]:
            self.enum_type = self.data["FILE_META"]["enum_type"]
            self.valued_enum = True
        
        self.enum = cpp.components.Enum(self.enum_name, True, self.enum_type)
        if self.generate_string_map:
            self.string_map = cpp.components.UnorderedMap(self.string_map_name, self.enum_name, "std::string", "const ")
        
    def __parse(self):
        for category_key in self.data:
            if category_key == "FILE_META":
                continue
            for item in self.data[category_key]:
                if item in ["SECTION_STRING", "SECTION_MASK"]:
                    continue
                self.__addEnum(category_key, item, self.data[category_key][item])

    # @brief Add an item to the enum
    def __addEnum(self, category, item_name, json_data):
        enum_name = ""
        enum_name += category.upper() + "_"
        
        item_name_parts = item_name.split("_")
        first = True
        for part in item_name_parts:
            if first:
                first = False
                enum_name += part.lower()
            else:
                enum_name += part.title()

        value = None
        if self.valued_enum:
            value = json_data["value"]

        comment = None
        if ("name" in json_data):
            comment = json_data["name"]
            if PRINT_OUTPUT:
                print(f'{enum_name} {comment}')
        else:
            if PRINT_OUTPUT:
                print(f'{enum_name}')

        if self.generate_string_map:
            self.string_map.addPair(f'{self.enum_name}::{enum_name}', f'"{enum_name}"')
        
        self.enum.addValue(enum_name, comment, value)

    def __write(self):
        file_name = OUT_FILE.split("/")[-1].split(".")[0]
        file = cpp.files.HppGenerator(file_name)
        if self.generate_string_map:
            file.addIncludes(["<unordered_map>", "<string>", "<cstdint>"])
        else:
            file.addIncludes(["<cstdint>"])
        if self.namespaced:
            file.enterNamespace(self.namespace)
        file.addLine("// NOLINTBEGIN(readability-identifier-naming)")
        file.addComponent(self.enum)
        file.addLine("// NOLINTEND(readability-identifier-naming)")
        
        if self.generate_string_map:
            file.addLine("")
            file.addLine("extern " + self.string_map.getDeclaration())
        
        if PRINT_OUTPUT: 
            print(file)
        file.save(OUT_FILE)
        
        if (self.generate_string_map):
            self.__write_string_map()

    def __write_string_map(self):
        file_name = STRING_MAP_OUT.split("/")[-1].split(".")[0]
        hpp_include_name = file_name = OUT_FILE.split("/")[-1].split(".")[0]
        file = cpp.files.CppGenerator(file_name)
        file.addIncludes([f'"{hpp_include_name}.hpp"'])
        if self.namespaced:
            file.enterNamespace(self.namespace)
        file.addLine("")
        file.addComponent(self.string_map)
        file.addLine("")

        if PRINT_OUTPUT:
            print(file)
        file.save(STRING_MAP_OUT)

if __name__ == "__main__":
    IdGenerator(IN_FILE, OUT_FILE)
    
print("Done.\n")