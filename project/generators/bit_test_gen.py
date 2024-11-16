import sys
import json
from cpp_gen_v2 import FileGenerator, Enum

assert len(sys.argv) == 3, "usage: bit_test_gen.py <metadata_json> <output_hpp_file>"

out_path = sys.argv[2]

out_hpp = FileGenerator("")
out_hpp.addIncludes("<cstdint>", "<string>")

input_json = json.load(open(sys.argv[1], "r"))
test_groups = input_json["tests"]
status_codes = input_json["statuses"]


test_group_enum = Enum("BitTestGroup", True, "uint8_t")

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
status_enum = Enum("BitTestStatus", True, "uint8_t")
status_enum.addDoxBrief("This enum contains the status codes for the Built-In Test (BIT) system.")
i = 0
for status in status_codes:
    abbreviation = status
    full_word = status_codes[status]["description"].replace(" ", "")
    status_enum.addMember(full_word, i)
    i += 1


out_hpp.addComponent(status_enum)
out_hpp.addComponent(test_group_enum)
out_hpp.addComponent(test_id_enum)
out_hpp.write(out_path)

# print(out_hpp)