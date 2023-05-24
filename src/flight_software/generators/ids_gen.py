import sys
import json

INDENT = "  "

if (len(sys.argv) != 4 or sys.argv[3] not in ["data", "log"]):
    print("data_ids_gen.py ARGS INVALID")
    sys.exit(-1)

mode = sys.argv[3] # 'data' or 'error'

print("\nGenerating data_ids.hpp with mode: " + mode)

file_top = """
// * * * * GENERATED AUTOMATICALLY WITH ids_gen.py * * * *

#ifndef {0}_IDS_HPP_
#define {0}_IDS_HPP_

namespace data {{
enum class {1}Id {{
""".format(mode.upper(), mode)


file_bottom = """};
}

#endif"""

in_data = open(sys.argv[1])
json_data = json.load(in_data)

data_item_list = []

def processSection(prefix: str, section: dict):
    for key in section:
        if key == "ID":
            continue
        
        data_item = section[key]
        if "ID" not in data_item:
            return
        data_item_list.append(prefix + "_" + data_item["ID"])
        print(" - " + data_item_list[-1])
    
for key in json_data:
    section = json_data[key]
    if "ID" not in section:
        continue
    processSection(section["ID"], json_data[key])


with open(sys.argv[2], "w") as out_file:
    out_file.write(file_top)
    for item in data_item_list:
        item = INDENT + item + ",\n"
        out_file.write(item)
    out_file.write(file_bottom)
    
print("Done.\n")