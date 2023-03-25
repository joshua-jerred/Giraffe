import json

class GiraffeApi:
    def __init__(self, config: dict, api_name: str = "Giraffe API"):
        self._api_name = api_name

        self._encoding = config["encoding"]

        self.get_options = []
        self.post_options = []
        self.put_options = []

        self._status = False

    def GetStatus(self):
        return self._status

    def SetStatus(self, status: bool):
        self._status = status

    def handleCode(self, code, message=""):
        if message == "":
            if code == 200:
                message = "OK"
            elif code == 201:
                message = "Created"
            elif code == 400:
                message = "Bad Request"
            elif code == 401:
                message = "Unauthorized"
            elif code == 403:
                message = "Forbidden"
            elif code == 404:
                message = "Not Found"
            elif code == 500:
                message = "Internal Server Error"
            elif code == 501:
                message = "Not Implemented"
        return (self._api_name + " API:" + message, code)
    
    def jsonPathToDict(self, path: str):
        with open(path) as f:
            return json.load(f)
        
    def DictToBytes(self, input):
        content_type = "application/json"
        return (bytes(json.dumps(input), self._encoding), content_type)
    
    def Get(self, path):
        return self.handleCode(501)
    
    def Post(self, path, data):
        return self.handleCode(501)
    
    def Put(self, path, data):
        return self.handleCode(501)
    
    def Delete(self, path):
        return self.handleCode(501)