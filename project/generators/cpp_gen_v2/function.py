from .component import Component

class Function(Component):
    def __init__(self, return_type, name, parameters, is_const = False):
        super().__init__()
        self.return_type = return_type
        self.name = name
        self.parameters = parameters
        self.is_const = is_const

        self.inner_components = []

    def get(self, indent = 0) -> str:
        self.clearLines()

        self.addLine(f'{self.return_type} {self.name}({self.parameters}) {"const" if self.is_const else  ""} {{', indent)
        for component in self.inner_components:
            self.addLine(component.get(indent + 1), 0, True)
        self.addLine("}", indent)
        return self.getAllLines()

    def addInsideComponent(self, component):
        if isinstance(component, Component):
            self.inner_components.append(component)
        elif isinstance(component, str):
            self.inner_components.append(Component.Line(component, 0))
        else:
            raise ValueError("Invalid component type")
    
        # self.inner_components.append(component)
    
    def getDeclaration(self, indent=0, extra_qualifiers = None) -> Component.Line:
        return Component.Line(f'{extra_qualifiers + " " if extra_qualifiers else ""}{self.return_type} {self.name}({self.parameters}){" const" if self.is_const else  ""};', indent)

if __name__ == "__main__":
    f = Function("void", "test", "testing", True)
    f.addInsideComponent(Component.Line("std::cout << \"Hello World!\" << std::endl;"))
    print(f.get(), end="")
    
    print(f.getDeclaration().get())