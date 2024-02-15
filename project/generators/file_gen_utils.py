from cpp_gen.files import AUTO_GEN_HEADER, AUTO_GEN_FOOTER

def headerFileHeader(file_name: str, includes: list = []): # file_name ex: structure - turns into structure.hpp
    includes_content = ""
    for item in includes:
        includes_content += f"#include {item}\n"

    output = AUTO_GEN_HEADER
    output += """

#ifndef {1}_HPP_
#define {1}_HPP_

{2}
""".format(file_name, file_name.upper(), includes_content)
    return output

def headerFileFooter(file_name: str):
    output = """
#endif // {0}_HPP_

""".format(file_name.upper())
    output += AUTO_GEN_FOOTER
    return output


def cppFileHeader(file_name: str, includes: list = []): # file_name ex: structure - turns into structure.hpp
    includes_content = ""
    for item in includes:
        includes_content += f"#include {item}\n"
    return AUTO_GEN_HEADER + """

{2}
""".format(file_name, file_name.upper(), includes_content)

def cppFileFooter(file_name: str): # file_name ex: structure - turns into structure.hpp
    return AUTO_GEN_FOOTER

def enterNameSpace(name):
    return f"namespace {name} {{\n"

def exitNameSpace(name):
    return f'}} // namespace {name}\n\n'

def struct(name: str, contents: list, indent = 2):
    struct = f"struct {name} {{\n"
    for item in contents:
        struct += f'{" " * indent}{item}\n'
    struct += "};\n\n"
    return struct