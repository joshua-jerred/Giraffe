from .file_headers import AUTO_GEN_HEADER, AUTO_GEN_FOOTER

INDENT_CHARS = "  "
DOXYGEN_FORMAT = "/// @"

class Component:
    class Line:
        def __init__(self, content, indent, skip_newline = False):
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
        # print("indent", indent)
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

    def addEmptyLine(self):
        self.__generated_lines.append(Component.Line("", 0, False))

    def getLines(self, additional_indent = 0):
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