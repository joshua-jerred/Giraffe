from .component import Component

class Block(Component):
    def __init__(self, wrap_block = False):
        super().__init__()
        self.wrap_block = wrap_block
        self.components = []

    def get(self, indent = 0):
        self.clearLines()

        if self.wrap_block:
            self.addLine('{', indent)

        self.addEmptyLine()
        for component in self.components:
            self.addLine(component.get(indent + 1), 0, True)
            self.addEmptyLine()

        if self.wrap_block:
            self.addLine("}", indent)

        return self.getAllLines()

    def addComponent(self, component):
        self.components.append(component)

if __name__ == "__main__":
    ns = Block("test")
    ns.addComponent(Component.Line("int x;", 0))
    ns.addComponent(Component.Line("int y;", 0))
    print(ns.get(), end="")