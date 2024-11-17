import sys
import json
from cpp_gen_v2 import FileGenerator, Enum, Map, TextBlock, Function, Namespace, Vector

assert len(sys.argv) == 4, "usage: bit_test_gen.py <metadata_json> <output_hpp_file> <output_cpp_file>"

out_path = sys.argv[2]
out_cpp_path = sys.argv[3]

out_hpp = FileGenerator("", pragma_once=True)
out_hpp.addIncludes("<cstdint>", "<string>", "<test_case.hpp>")

input_json = json.load(open(sys.argv[1], "r"))
test_groups = input_json["tests"]
status_codes = input_json["statuses"]

# Test Group Generation
test_group_enum = Enum("TestGroupId", True, "uint8_t")
test_group_enum.addDoxBrief("Identifiers for the different groups of BIT tests.")
# test_group_enum.addMember("Unknown", "0", "Unknown test group")
test_group_enum_string_map = Map("TestGroupIdToStringMap", "TestGroupId", "const std::string", "static const std::map")
test_group_enum_string_map.addDoxBrief("This map converts the TestGroupId enum to a string.")
test_group_enum_to_string_func = Function("std::string", "testGroupIdToString", "TestGroupId groupId")
test_group_enum_to_string_func.addDoxBrief("This function converts a TestGroupId enum to a string.")

# Test ID Generation
test_id_enum = Enum("TestId", True, "uint16_t")
test_id_enum.addDoxBrief("This enum contains the test ids for the Built-In Test (BIT) system.")
# test_id_enum.addMember("Unknown", "0x0000", "Unknown test id")
test_id_enum_string_map = Map("TestIdToStringMap", "TestId", "const std::string", "static const std::map")
test_id_enum_string_map.addDoxBrief("This map converts the TestId enum to a string.")
test_id_enum_to_string_func = Function("std::string", "testIdToString", "TestId testId")
test_id_enum_to_string_func.addDoxBrief("This function converts a TestId enum to a string.")

# Test Status Generation
status_enum = Enum("TestStatus", True, "uint8_t")
status_enum.addDoxBrief("This enum contains the status codes for the Built-In Test (BIT) system.")
status_string_map = Map("TestStatusToStringMap", "TestStatus", "const std::string", "static const std::map")
status_string_map.addDoxBrief("This map converts the TestStatus enum to a string.")
status_enum_to_string_func = Function("std::string", "testStatusToString", "TestStatus status")
status_enum_to_string_func.addDoxBrief("This function converts a TestStatus enum to a string.")


# A dictionary that maps TestGroupIds to an array of TestIds
group_test_list_vectors = {
}

i = 1
for group_label in test_groups:
    group_data = test_groups[group_label]
    group_description = group_data["description"]
    tests = group_data["tests"]
    test_group_enum.addMember(group_label, i, doxygen_brief=group_description)
    test_group_enum_string_map.addPair(f'TestGroupId::{group_label}', f'"{group_label}"')
    i += 1

    group_test_list_vectors[group_label] = []
    

    for test in tests:
        name = f'{group_label}_{test["title"].title().replace(" ", "")}Test'
        test_id_enum.addMember(name, test["id"])
        test_id_enum_string_map.addPair(f'TestId::{name}', f'"{name}"')

        group_test_list_vectors[group_label].append(name)

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
namespace.addComponent(test_group_enum_to_string_func.getDeclaration(extra_qualifiers="extern"))

namespace.addComponent(test_id_enum)
namespace.addComponent(test_id_enum_to_string_func.getDeclaration(extra_qualifiers="extern"))

out_hpp.addComponent(namespace)
out_hpp.write(out_path)

out_cpp = FileGenerator("")
out_cpp.addIncludes("<map>", "<vector>", "", "",  '"giraffe_assert.hpp"', '"bit_types.hpp"')

cpp_namespace = Namespace("bit")
cpp_namespace.addComponent(status_string_map)
status_enum_to_string_func.addInsideComponent('if (!TestStatusToStringMap.contains(status)) {')
status_enum_to_string_func.addInsideComponent('giraffe_assert(false);')
status_enum_to_string_func.addInsideComponent('return TestStatusToStringMap.at(TestStatus::Unknown);')
status_enum_to_string_func.addInsideComponent('}')
status_enum_to_string_func.addInsideComponent('return TestStatusToStringMap.at(status);')
cpp_namespace.addComponent(status_enum_to_string_func)

cpp_namespace.addComponent(test_group_enum_string_map)
test_group_enum_to_string_func.addInsideComponent('if (!TestGroupIdToStringMap.contains(groupId)) {')
test_group_enum_to_string_func.addInsideComponent('giraffe_assert(false);')
test_group_enum_to_string_func.addInsideComponent('return "Unknown";')
test_group_enum_to_string_func.addInsideComponent('}')
test_group_enum_to_string_func.addInsideComponent('return TestGroupIdToStringMap.at(groupId);')
cpp_namespace.addComponent(test_group_enum_to_string_func)

cpp_namespace.addComponent(test_id_enum_string_map)
test_id_enum_to_string_func.addInsideComponent('if (!TestIdToStringMap.contains(testId)) {')
test_id_enum_to_string_func.addInsideComponent('giraffe_assert(false);')
# test_id_enum_to_string_func.addInsideComponent('return TestIdToStringMap.at(TestId::Unknown);')
test_id_enum_to_string_func.addInsideComponent('return "Unknown";')
test_id_enum_to_string_func.addInsideComponent('}')
test_id_enum_to_string_func.addInsideComponent('return TestIdToStringMap.at(testId);')
cpp_namespace.addComponent(test_id_enum_to_string_func)


# Now that we have all of the base enums and maps, we can generate the test vectors
VECTOR_PREFIX = "const std::vector<TestCase> "
for group_label in group_test_list_vectors:
    group_test_list = group_test_list_vectors[group_label]
    group_test_vector = Vector(f"BIT_TEST_GROUP_{group_label}", "TestCase", extra_qualifiers="static")
    group_test_vector.addDoxBrief(f"Test cases for the {group_label} group.")
    for test in group_test_list:
        group_test_vector.addString(f"{{{group_label}, TestId::{test}, TestStatus::Unknown}}")
        # print(test, group_test_list)
    cpp_namespace.addComponent(group_test_vector)

# for group_label in group_test_list_vectors:
    # group_test_list = group_test_list_vectors[group_label]
# print("123456abcde", group_test_vector.get())

out_cpp.addComponent(cpp_namespace)
out_cpp.write(out_cpp_path)