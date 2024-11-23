"""Basic templater for C++ code generation."""

import re

class Templater:
    def __init__(self, file_path):
        self.file_path = file_path
        self.start_tag_pos = None
        self.end_tag_pos = None
        self.first_half = None
        self.second_half = None

    def template(self, tag:str, replacement:str, write_to_file:bool = True):
        with open(self.file_path, "r") as file:
            while True:
                line = file.readline()
                if not line:
                    break
                if f'//<{{{{{tag}}}}}@' in line:
                    print("found start tag")
                    self.start_tag_pos = file.tell()
                if f'//@{{{{{tag}}}}}>' in line:
                    print("found end tag")
                    self.end_tag_pos = file.tell() - len(line) - 1

            file.seek(0)
            self.first_half = file.read(self.start_tag_pos)
            file.seek(self.end_tag_pos)
            self.second_half = file.read()

        whole_file = self.first_half + replacement + self.second_half
        with open(self.file_path, "w") as file:
            file.write(whole_file)