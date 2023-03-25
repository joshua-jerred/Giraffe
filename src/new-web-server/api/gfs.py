# Giraffe Flight Software API

class GFS:
    def __init__(self, setup_file_path, encoding="utf-8"):
        self.encoding = encoding
        self.setup_file_path = setup_file_path

        self.get_options = []
        self.post_options = []
        self.put_options = []

        self.status = True # True if connected to GFS

    def GetStatus(self):
        return self.status