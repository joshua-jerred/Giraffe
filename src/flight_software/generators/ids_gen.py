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

PRINT_OUTPUT = True if os.getenv('PRINT_OUTPUT') == "true" else False

class IdGenerator:
    def __init__(self, in_file, out_file) -> None:
        self.enum = None
        
        self.in_path = in_file
        self.out_path = out_file
        self.data = None
        
        self.namespace = ""
        self.enum_name = ""
        
        self.__loadData()
        self.__parse()
        self.__write()

    def __loadData(self):
        f = open(self.in_path, "r")
        self.data = json.load(f)
        
        self.namespace = self.data["FILE_META"]["namespace"]
        self.enum_name = self.data["FILE_META"]["enum_name"]
        self.enum = cpp.components.Enum(self.enum_name)
        
    def __parse(self):
        for category_key in self.data:
            if category_key == "FILE_META":
                continue
            for item in self.data[category_key]:
                self.__addEnum(category_key, item, self.data[category_key][item])

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
                
        comment = None
        if ("name" in json_data):
            comment = json_data["name"]
            if PRINT_OUTPUT:
                print(f'{enum_name} {comment}')
        else:
            if PRINT_OUTPUT:
                print(f'{enum_name}')

        self.enum.addValue(enum_name, comment)

    def __write(self):
        file_name = OUT_FILE.split("/")[-1].split(".")[0]
        file = cpp.files.HppGenerator(file_name)
        file.enterNamespace(self.namespace)
        file.addComponent(self.enum)
        print(file)
        file.save(OUT_FILE)

if __name__ == "__main__":
    IdGenerator(IN_FILE, OUT_FILE)

# with open(sys.argv[2], "w") as out_file:
    # out_file.write(file_top)
    # for item in data_item_list:
        # item = INDENT + item + ",\n"
        # out_file.write(item)
    #out_file.write(file_bottom)
    
print("Done.\n")