import sys
import json
from cpp_gen_v2 import FileGenerator, Enum

assert len(sys.argv) == 3, "usage: bit_test_gen.py <metadata_json> <output_hpp_file>"

out_path = sys.argv[2]

out_hpp = FileGenerator("")
out_hpp.addIncludes("<cstdint>", "<string>")

input_json = json.load(open(sys.argv[1], "r"))
test_groups = input_json["tests"]

enum = Enum("BitTestId", True, "uint16_t")
enum.addDoxBrief("This enum contains all test ids for the Built-In Test (BIT) system.")

for group_label in test_groups:
    group_data = test_groups[group_label]
    group_description = group_data["description"]
    tests = group_data["tests"]

    # (name:str, id:int)
    test_name_id_pairs = []

    for test in tests:
        name = f'{group_label}_{test["title"].title().replace(" ", "")}Test'
        enum.addMember(name, test["id"])


out_hpp.addComponent(enum)
out_hpp.write(out_path)

# print(out_hpp)