# A set of utilities to generate C++ code.

AUTO_GEN_HEADER = """/**
 *
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 *
 *
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 * 
 * 
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       2023-06-30
 * @copyright  2023 (license to be defined)
 * =*=======================*=
 *
 * @verbatim
 */
"""

AUTO_GEN_FOOTER = """
/**
 * @endverbatim
 * 
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */"""

AUTO_GEN_LINE = "// * * * * GENERATED AUTOMATICALLY, DO NOT MANUALLY MODIFY * * * *"
NL = "\n"

class Generator:
    def __init__(self, name: str, indent):
        self.name = name
        self.indent = indent
        self.content = ""
        self.indent_level = 0
        self.namespace = []
        self.finished = False
        
        # start of file content
        self.addLine(AUTO_GEN_HEADER)
        self.addLine("")
    
    def addLine(self, line: str):
        if (self.content != ""):
            self.content += NL + self.indent * self.indent_level
        self.content += line
        
    def addLines(self, lines: list):
        for line in lines:
            self.addLine(line)

    def increaseIndent(self):
        self.indent_level += 1
        
    def decreaseIndent(self):
        if (self.indent_level > 0):
            raise Exception("Indent level cannot be negative.")

    def enterNamespace(self, namespace: str):
        self.namespace.append(namespace)
        self.addLine("namespace " + namespace + " {")
        # self.increase_indent() google style guide
        
    def addIncludes(self, includes: list):
        for include in includes:
            self.addLine("#include " + include)
        self.addLine("")

    def exitNamespace(self):
        if (len(self.namespace) == 0):
            raise Exception("Cannot exit namespace, no namespace entered.")
        self.addLine("} // namespace " + self.namespace.pop())
        # self.decrease_indent() google style guide

    def __repr__(self):
        self.finish()
        return self.content

    def closeAllNamespaces(self):
        """Closes all namespaces.
        """
        while (len(self.namespace) > 0):
            self.exitNamespace()

    def finish(self):
        if (self.finished):
            return
        self.finished = True
        
        self.closeAllNamespaces()
        
        if (self.indent_level != 0):
            raise Exception("Indent level must be zero to finish.")
        
        self.addLine("")
        self.addLine(AUTO_GEN_FOOTER)
    
    def addComponent(self, component):
        self.addLines(component.getLines())
    
    def save(self, path: str):
        self.finish()
        f = open(path, "w")
        f.write(str(self))
        f.close()

class CppGenerator(Generator):
    def __init__(self, name: str, indent = "  "):
        super().__init__(name, indent)
        
    def __repr__(self):
        self.finish()
        return super().__repr__()
    
    def finish(self):
        if (self.finished):
            return
        
        super().closeAllNamespaces()
        super().finish()
    
    def save(self, path: str):
        self.finish()
        super().save(path)

class HppGenerator(Generator):
    def __init__(self, name: str, indent = "  "):
        super().__init__(name, indent)
        self.__addIncludeGuard()

    def __addIncludeGuard(self):
        self.addLine(f'#ifndef {self.name.upper()}_HPP_')
        self.addLine(f'#define {self.name.upper()}_HPP_')
        self.addLine("")
    
    def __addEndIncludeGuard(self):
        self.addLine("")
        self.addLine(f'#endif // {self.name.upper()}_HPP_')
    
    def __repr__(self):
        self.finish()
        return super().__repr__()
    
    def finish(self):
        if (self.finished):
            return
        
        super().closeAllNamespaces()
        self.__addEndIncludeGuard()
        super().finish()
    
    def save(self, path: str):
        self.finish()
        super().save(path)
        
if __name__ == "__main__":
    x = HppGenerator("test")
    
    print(x)

