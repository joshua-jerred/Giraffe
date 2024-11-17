import sys
import json
from cpp_gen_v2 import FileGenerator, Enum, Map, TextBlock, Function, Namespace

assert len(sys.argv) == 4, "usage: bit_test_gen.py <metadata_json> <output_hpp_file> <output_cpp_file>"

out_path = sys.argv[2]
out_cpp_path = sys.argv[3]

out_hpp = FileGenerator("", pragma_once=False)
out_hpp.addIncludes("<cstdint>", "<string>")

input_json = json.load(open(sys.argv[1], "r"))
test_groups = input_json["tests"]
status_codes = input_json["statuses"]

test_group_enum = Enum("BitTestGroup", True, "uint8_t")
test_group_enum.addDoxBrief("Identifiers for the different groups of BIT tests.")

# Generate the Test IDs
test_id_enum = Enum("BitTestId", True, "uint16_t")
test_id_enum.addDoxBrief("This enum contains the test ids for the Built-In Test (BIT) system.")
for group_label in test_groups:
    group_data = test_groups[group_label]
    group_description = group_data["description"]
    tests = group_data["tests"]
    
    test_group_enum.addMember(group_description.replace(" ", ""), doxygen_brief=group_description)

    # (name:str, id:int)
    test_name_id_pairs = []

    for test in tests:
        name = f'{group_label}_{test["title"].title().replace(" ", "")}Test'
        test_id_enum.addMember(name, test["id"])


# Generate the Status Codes
status_enum = Enum("TestStatus", True, "uint8_t")
status_enum.addDoxBrief("This enum contains the status codes for the Built-In Test (BIT) system.")
status_string_map = Map("TestStatusToStringMap", "TestStatus", "const std::string", "static const std::map")
status_string_map.addDoxBrief("This map converts the TestStatus enum to a string.")
status_enum_to_string_func = Function("std::string", "testStatusToString", "TestStatus status")

i = 0
for status in status_codes:
    abbreviation = status
    full_word = status_codes[status]["description"].replace(" ", "")
    status_enum.addMember(full_word, i, status)
    status_string_map.addPair(f'TestStatus::{full_word}', f'"{abbreviation}"')
    i += 1

# Write to the output file
namespace = Namespace("bit")

namespace.addComponent(status_enum)
namespace.addComponent(status_enum_to_string_func.getDeclaration(extra_qualifiers="extern"))
# out_hpp.addComponent(TextBlock(""))
namespace.addComponent(test_group_enum)
namespace.addComponent(test_id_enum)

out_hpp.addComponent(namespace)
out_hpp.write(out_path)

out_cpp = FileGenerator("")
out_cpp.addIncludes("<map>", '"bit_test_enums.hpp"')

cpp_namespace = Namespace("bit")
cpp_namespace.addComponent(status_string_map)
status_enum_to_string_func.addInsideComponent('return;')
cpp_namespace.addComponent(status_enum_to_string_func)

out_cpp.addComponent(cpp_namespace)
out_cpp.write(out_cpp_path)