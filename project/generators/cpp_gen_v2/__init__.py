import os

from .file_headers import AUTO_GEN_HEADER, AUTO_GEN_FOOTER
from .component import Component
from .namespace import Namespace
from .function import Function 
from .vector import Vector
from .templater import Templater
from .block import Block

INDENT_CHARS = "  "
DOXYGEN_FORMAT = "/// @"

class Component:
    class Line:
        def __init__(self, content, indent = 0, skip_newline = False):
            self.content = content
            self.indent = indent
            self.skip_newline = skip_newline
            # print(self.indent, skip_newline)

        def get(self, additional_indent = 0, add_newline = True):
            out = f"{INDENT_CHARS * (self.indent + additional_indent)}{self.content}"
            if add_newline and not self.skip_newline:
                out += "\n"
            # print(self.indent)
            return out
        
        def getIndentation(self):
            return self.indent

    def __init__(self) -> None:
        self.doxygen_brief = None

        self.__generated_lines = [] # List of Line objects

    def addLine(self, content:str, indent:int, skip_newline = False):
        # If there is a doxygen brief available, add it as the first line
        if len(self.__generated_lines) == 0 and self.doxygen_brief is not None:
            self.__generated_lines.append(Component.Line(self.doxygen_brief, indent, skip_newline))

        self.__generated_lines.append(Component.Line(content, indent, skip_newline))

    def addLineComponent(self, component:Line, indent:int):
        # print("indent-b", indent, type(component))
        
        if len(self.__generated_lines) == 0 and self.doxygen_brief is not None:
            self.__generated_lines.append(Component.Line(self.doxygen_brief, indent))
        # print("indent-c", indent)
        self.__generated_lines.append(component)
        # print(self.__generated_lines[-1])

    def getLines(self, additional_indent = 0) -> list:
        return [line.get(additional_indent) for line in self.__generated_lines]

    def clearLines(self):
        self.__generated_lines = []

    def getAllLines(self) -> str:
        out = ""
        for line in self.__generated_lines:
            # print(line.get(), end="")
            # print(line.getIndentation())
            out += line.get()
        return out

    def addDoxBrief(self, brief):
        self.doxygen_brief = f"{DOXYGEN_FORMAT}brief {brief}"

    def getDoxBrief(self):
        return f" {self.doxygen_brief if self.doxygen_brief is not None else ''}"

class TextBlock(Component):
    def __init__(self, component):
        super().__init__()
        self.component:Component.Line = Component.Line(
            component, 0, False
        )

    def setComponent(self, component):
        self.component = component

    def get(self, indent = 0):
        return self.component.get(indent)

class Enum(Component):
    class Member(Component):
        def __init__(self, name, value = None, doxygen_brief = None):
            super().__init__()
            # if doxygen_brief is not None:
                # super().addDoxBrief(doxygen_brief)
            self.dox_brief = doxygen_brief
            self.name = name
            self.value = value

        def __str__(self):
            return self.get()

        def getDoxygenBrief(self):
            if self.dox_brief is None:
                return ""
            return f" {DOXYGEN_FORMAT}brief {self.dox_brief}"

        def get(self, indent = 0):
            self.clearLines()
            if self.value is not None:
                self.addLine(f"{self.name} = {self.value},{self.getDoxygenBrief()}", indent, False)
            else:
                self.addLine(f"{self.name},", indent, False)

            return self.getAllLines()


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
            self.addLine(member.get(indent + 1), 0, True)
            # self.addLine(member.get(indent + 1), 0)
        self.addLine("};", indent)

        return self.getAllLines()

    def addMember(self, name, value = None, doxygen_brief = None):
        self.members.append(Enum.Member(name, value, doxygen_brief))

class Map(Component):
    class Pair:
        def __init__(self, key, value, trailing_comma = True):
            self.key = key
            self.value = value
            self.trailing_comma = trailing_comma

        def get(self):
            return f'{{{self.key}, {self.value}}}' + ("," if self.trailing_comma else "")

    def __init__(self, name, key_type, value_type, map_type = "std::map", clang_format_off = False):
        super().__init__()
        self.map_type = map_type
        self.name = name
        self.key_type = key_type
        self.value_type = value_type
        self.clang_format_off = clang_format_off

        self.pairs = []

    def get(self, indent = 0) -> str:
        self.clearLines()
        
        if self.clang_format_off:
            self.addLine("// clang-format off", indent)

        self.addLine(f'{self.map_type}<{self.key_type}, {self.value_type}> {self.name}{{', indent)
        for pair in self.pairs:
            self.addLine(pair.get(), indent + 1)
        self.addLine("};", indent, not self.clang_format_off)
        
        if self.clang_format_off:
            self.addLine("// clang-format on", indent, True)

        return self.getAllLines()

    def getDeclaration(self, indent = 0, front_qualifiers="") -> str:
        return f'{front_qualifiers} {self.map_type}<{self.key_type}, {self.value_type}> {self.name};'

    def addPair(self, key, value):
        self.pairs.append(Map.Pair(key, value))

class FileGenerator:
    def __init__(self, filename, space_between_components = 1,
                 pragma_once = False, auto_gen_header = True, 
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

        if self.auto_gen_header:
            output += file_headers.AUTO_GEN_HEADER
            output += self.__getLineSpacing()

        if self.pragma_once:
            output += f"#pragma once\n{self.__getLineSpacing()}"

        for include in self.includes:
            output += f"{include}\n"

        # first = True
        for component in self.components:
            # print(type(component))
            output += self.__getLineSpacing()
            # first = False
            output += component.get(0)

        if self.auto_gen_footer:
            output += file_headers.AUTO_GEN_FOOTER
        return output


    def addComponent(self, component):
        self.components.append(component)

    def addIncludes(self, include, *args):
        if include == "":
            self.includes.append("")
        else:
            self.includes.append(f"#include {include}")

        for arg in args:
            if arg == "":
                self.includes.append("")
            else:
                self.includes.append(f"#include {arg}")

    def write(self, filepath):
        with open(filepath, "w") as file:
            file.write(self.__generate())

        if os.getcwd().split("/")[-1].lower() == "giraffe":
            print(f"File written to {filepath}")

if __name__ == "__main__":
    # a few quick tests
    namespace = Namespace("test_namespace")
    out = namespace.getAllLines()