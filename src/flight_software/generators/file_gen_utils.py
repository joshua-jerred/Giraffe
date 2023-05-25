def headerFileHeader(file_name: str, includes: list = []): # file_name ex: structure - turns into structure.hpp
    includes_content = ""
    for item in includes:
        includes_content += f"#include {item}\n"
    return """// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *

#ifndef {1}_HPP_
#define {1}_HPP_

{2}
""".format(file_name, file_name.upper(), includes_content)

def headerFileFooter(file_name: str):
    return """
#endif // {0}_HPP_

// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *
""".format(file_name.upper())

def cppFileHeader(file_name: str, includes: list = []): # file_name ex: structure - turns into structure.hpp
    includes_content = ""
    for item in includes:
        includes_content += f"#include {item}\n"
    return """// * * * AUTOMATICALLY GENERATED WITH CMake/Python * * *

{2}
""".format(file_name, file_name.upper(), includes_content)

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