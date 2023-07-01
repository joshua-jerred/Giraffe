class Component:
    def __init__(self):
        self.__indent = "  "
        self.__indent_level = 0
        self.__content_lines = []
        self.__closing_bracket_stack = []

    def getLines(self): # for iteration of each line
        """Returns a list of strings (of code) with their appropriate 
        indentation. This is used for adding lines in the file classes.

        Returns:
            list: a list of strings that are lines of code
        """
        self.__finish()
        return self.__content_lines

    def addLine(self, line):
        """used for adding lines to a component without worrying about
        indentation

        Args:
            line (str): a line of code
        """
        
        line_str = ""
        if (len(self.__content_lines) > 0):
            line_str += f"{self.__indent * self.__indent_level}"
        line_str += line
        self.__content_lines.append(line_str)
    
    def addLineWithBracket(self, line, closing_bracket = "}", open_bracket = "{"):
        """Same as addLine, but this helps to automatically add closing brackets
        and indentation.
        Args:
            line (str): a line of code
            closing_bracket (str, optional): closing bracket, ex '}', or '};'. 
                Defaults to "}".
            open_bracket (str, optional): opening bracket style. Defaults to "{".
        """
        self.addLine(f"{line} {open_bracket}")
        self.__increaseIndent()
        self.__closing_bracket_stack.append(closing_bracket)
    
    def leaveBracket(self):
        """Leaves a previously entered bracket. Takes care of indentation.

        Raises:
            Exception: _description_
        """
        self.__decreaseIndent()
        if (len(self.__closing_bracket_stack) == 0):
            raise Exception("Cannot leave bracket, no bracket entered.")
        self.addLine(self.__closing_bracket_stack.pop())
    
    def addLines(self, lines):
        """Used to add multiple lines of code at once.

        Args:
            lines (_type_): _description_
        """
        for line in lines:
            self.addLine(line)

    def __repr__(self)-> str:
        """Calls __finish() and returns the string representation.

        Returns:
            str: String representation of the component.
        """
        self.__finish()
        ret_str = ""
        first = True
        for line in self.getLines():
            if not first:
                ret_str += "\n"
            ret_str += line
            first = False
        return ret_str
    
    def __increaseIndent(self):
        """INTERNAL USE ONLY RECOMMENDED: Increases the indent level by one.
        """
        self.__indent_level += 1
        
    def __decreaseIndent(self):
        """INTERNAL USE ONLY RECOMMENDED: Decreases the indent level by one.

        Raises:
            Exception: If the indent level can not be decreased.
        """
        if (self.__indent_level <= 0):
            raise Exception("Indent level cannot be negative.")
        self.__indent_level -= 1
    
    def __finish(self):
        """INTERNAL USE ONLY RECOMMENDED: Finishes the component by adding
        remaining closing brackets and verifying that the indent level is zero.

        This often needs to be overridden by subclasses.
        
        Raises:
            Exception: If the indent level is not zero after finishing.
        """
        while (len(self.__closing_bracket_stack) > 0):
            self.leaveBracket()
        
        if (self.__indent_level != 0):
            raise Exception("Indent level not zero after finishing component.")

class EnumValue:
    def __init__(self, value, comment, numeric_value):
        self.value = value
        self.last = False
        self.comment = comment
        self.numeric_value = numeric_value
    
    def getLine(self):
        ret_val = f'{self.value}{"" if self.numeric_value == None else "="+self.numeric_value}{"," if not self.last else ""}'
        if self.comment != None:
            ret_val += f'{" " if not self.last else "  "}// {self.comment}'
        return ret_val
    
    def setLast(self):
        self.last = True

class Enum(Component):
    def __init__(self, name, enum_class = True, enum_type = None):
        super().__init__()
        self.name = name
        self.values = []
        
        if (enum_class):
            self.addLineWithBracket("enum class " + name + f'{"" if enum_type==None else " : "+enum_type}', "};")
        else:
            self.addLineWithBracket(f"enum {name}")

    def addValue(self, enum_value_name, comment = None, value = None): # ex: pass ing 'ENUM_ITEM' without a comma
        self.values.append(EnumValue(enum_value_name, comment, value))

    def addValues(self, enum_value_names): # ex: pass in ['ENUM_ITEM', 'ENUM_ITEM2', 'ENUM_ITEM3']
        for enum_value_name in enum_value_names:
            self.addValue(enum_value_name)
    
    def getLines(self):
        self.__finish()
        return super().getLines()
    
    def __repr__(self) -> str:
        self.__finish()
        return super().__repr__()
    
    def __finish(self):
        if (len(self.values) > 0):
            self.values[-1].setLast()
        while (len(self.values) > 0):
            self.addLine(self.values.pop(0).getLine())

class UnorderedMapPair:
    def __init__(self, key, value, comment = None):
        self.key = key
        self.value = value
        self.last = False
        self.comment = comment
    
    def getLine(self):
        ret_val = f'{{{self.key},{self.value}}}{"," if not self.last else ""}'
        if self.comment != None:
            ret_val += f'{" " if not self.last else "  "}// {self.comment}'
        return ret_val
    
    def setLast(self):
        self.last = True

class UnorderedMap(Component):
    def __init__(self, name, key_type, value_type, additional = ""):
        super().__init__()
        self.name = name
        self.key_type = key_type
        self.value_type = value_type
        self.addLineWithBracket(f"{additional}std::unordered_map<{key_type}, {value_type}> {self.name}", "};")
        self.values:tuple = []

    def __repr__(self) -> str:
        self.__finish()
        return super().__repr__()
    
    def __finish(self):
        if (len(self.values) > 0):
            self.values[-1].setLast()
        while (len(self.values) > 0):
            self.addLine(self.values.pop(0).getLine())
            
    def addPair(self, key, value, comment = None):
        self.values.append(UnorderedMapPair(key, value, comment))
    
    def getLines(self):
        self.__finish()
        return super().getLines()
    
    def __repr__(self) -> str:
        self.__finish()
        return super().__repr__()
    
    def __finish(self):
        if (len(self.values) > 0):
            self.values[-1].setLast()
        while (len(self.values) > 0):
            self.addLine(self.values.pop(0).getLine())

# quick test
if __name__ == "__main__":
    x = Enum("test")
    x.addValues(["ENUM_ITEM", "ENUM_ITEM2", "ENUM_ITEM3"])
    x.addValue("ENUM_ITEM4", "comment")
    x.addValue("ENUM_ITEM5", "comment")
    print(x.getLines())
    print(x)
    print()
    
    x = UnorderedMap("test", "int", "int")
    x.addPair("1", "2")
    x.addPair("3", "4")
    print(x.getLines())
    print(x)