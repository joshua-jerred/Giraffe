from .component import Component

class Vector(Component):
    def __init__(self, name, type, extra_qualifiers, std_type = "std::vector"):
        """Lazy C++ vector model. Should work fine with arrays too.
            extra_qualifiers = "const", "constexpr", "static". They get placed in front. 
        """
        super().__init__()
        self.name = name
        self.extra_qualifiers = extra_qualifiers
        self.type = type
        self.std_type = std_type

        self.components = []

    def get(self, indent = 0):
        self.clearLines()

        # const std::vector<int> test {
        self.addLine(f'{self.extra_qualifiers} {self.std_type}<{self.type}> {self.name} {{', indent)

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