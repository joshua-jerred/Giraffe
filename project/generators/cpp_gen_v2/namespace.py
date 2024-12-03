from .component import Component

class Namespace(Component):
    def __init__(self, name):
        super().__init__()
        self.name = name
        self.components = []

    def get(self, indent = 0):
        self.clearLines()
        self.addLine(f'namespace {self.name} {{', indent)

        self.addEmptyLine()
        for component in self.components:
            self.addLine(component.get(indent + 1), 0, True)
            self.addEmptyLine()

        self.addLine("}", indent)

        return self.getAllLines()

    def addComponent(self, component):
        self.components.append(component)

if __name__ == "__main__":
    ns = Namespace("test")
    ns.addComponent(Component.Line("int x;", 0))
    ns.addComponent(Component.Line("int y;", 0))
    print(ns.get(), end="")