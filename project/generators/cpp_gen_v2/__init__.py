from .file_headers import AUTO_GEN_HEADER, AUTO_GEN_FOOTER

INDENT_CHARS = "  "
DOXYGEN_FORMAT = "/// @"

class Component:
    class Line:
        def __init__(self, content, indent):
            self.content = content
            self.indent = indent

        def get(self, additional_indent = 0):
            return f"{INDENT_CHARS * self.indent}{self.content}\n"

    def __init__(self) -> None:
        self.__generated_lines = []
        self.doxygen_brief = None

    def addLine(self, content, indent):
        if len(self.__generated_lines) == 0 and self.doxygen_brief is not None:
            self.__generated_lines.append(Component.Line(self.doxygen_brief, indent))
        self.__generated_lines.append(Component.Line(content, indent))

    def getLines(self, additional_indent = 0):
        return [line.get(additional_indent) for line in self.__generated_lines]

    def clearLines(self):
        self.__generated_lines = []

    def getAllLines(self) -> str:
        out = ""
        for line in self.__generated_lines:
            out += line.get()
        return out

    def addDoxBrief(self, brief):
        self.doxygen_brief = f"{DOXYGEN_FORMAT} {brief}"

class Enum(Component):
    class Member:
        def __init__(self, name, value = None):
            self.name = name
            self.value = value

        def __str__(self):
            self.get()

        def get(self):
            if self.value is not None:
                return f"{self.name} = {self.value},"
            else:
                return f"{self.name},"


    def  __init__(self, name, enum_class = True, enum_type = None):
        super().__init__()
        self.name = name
        self.enum_class = enum_class
        self.enum_type = enum_type

        self.members = []

    def __str__(self):
        return self.get()

    def get(self, indent = 0) -> str:
        self.clearLines()

        self.addLine(f'enum class {self.name} : {self.enum_type} {{', indent)
        for member in self.members:
            self.addLine(member.get(), indent + 1)
        self.addLine("};", indent)

        return self.getAllLines()

    def addMember(self, name, value):
        self.members.append(Enum.Member(name, value))


class FileGenerator:
    def __init__(self, filename, space_between_components = 1,
                 pragma_once = True, auto_gen_header = True, 
                 auto_gen_footer = True):
        self.filename = filename
        self.includes = []
        self.components = []
        self.space_between_components = space_between_components
        self.pragma_once = pragma_once
        self.auto_gen_header = auto_gen_header
        self.auto_gen_footer = auto_gen_footer

    def __repr__(self) -> str:
        return self.__generate()
    
    def __getLineSpacing(self):
        return "\n" * self.space_between_components

    def __generate(self) -> str:
        output = ""
        first = True

        if self.auto_gen_header:
            output += file_headers.AUTO_GEN_HEADER
            output += self.__getLineSpacing()

        if self.pragma_once:
            output += f"#pragma once\n{self.__getLineSpacing()}"

        for include in self.includes:
            output += f"{include}\n"

        output += self.__getLineSpacing()

        for component in self.components:
            if not first:
                self.__getLineSpacing()
            first = False
            output += component.get()

        if self.auto_gen_footer:
            output += file_headers.AUTO_GEN_FOOTER

        return output

        # for line in lines:
            # print(line)
        # print(lines)
        # for line in lines:
            # out_string += line.get()

    def addComponent(self, component):
        self.components.append(component)

    def addIncludes(self, include, *args):
        self.includes.append(f"#include {include}")
        for arg in args:
            self.includes.append(f"#include {arg}")

    def write(self, filepath):
        with open(filepath, "w") as file:
            file.write(self.__generate())