from .component import Component

class Vector(Component):
    def __init__(self, name, type = None, extra_qualifiers = None, std_type = "std::vector", equal_sign = True):
        """Lazy C++ vector model. Should work fine with arrays too.
            extra_qualifiers = "const", "constexpr", "static". They get placed in front. 
        """
        super().__init__()
        self.name = name
        self.extra_qualifiers = extra_qualifiers
        self.type = type
        self.std_type = std_type
        self.equal_sign = equal_sign

        self.components = []

    def get(self, indent = 0):
        self.clearLines()

        first_line = f'{self.extra_qualifiers} ' if self.extra_qualifiers is not None else ''
        first_line += f'{self.std_type}' + (f'<{self.type}>' if self.type is not None else '') + f' {self.name}'
        first_line += " = {" if self.equal_sign else '{'
        self.addLine(first_line, indent)

        # content of the vector
        for component in self.components:
            self.addLine(f'{component},', indent + 1)

        self.addLine("};", indent)

        return self.getAllLines()

    def addString(self, obj:str):
        self.components.append(obj)

if __name__ == "__main__":
    ns = Vector("test", "int", "static")
    ns.addString("123")
    ns.addString("123")
    ns.addString("123")
    # ns.addObject(Component.Line("int y;", 0))
    print(ns.get(), end="")