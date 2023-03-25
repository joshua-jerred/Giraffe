# Giraffe Flight Software API
import api.api

class GFS(api.api.GiraffeApi):
    def __init__(self, config: dict):
        api.api.GiraffeApi.__init__(self, config, "gfs")

        self.get_options = ["config"]
        self.post_options = []
        self.put_options = []

        self.config = self.jsonPathToDict("./data/gfs-configuration.json")

        self.SetStatus(True)


        
    def handleGetConfig(self, api_parts):
        print(api_parts)
        if len(api_parts) == 2:                 # GET /gfs/config
            return self.DictToBytes(self.config)
        else:
            return self.handleCode(400)

    def Get(self, api_parts):
        if api_parts[0] != "gfs" or len(api_parts) < 2:
            return self.handleCode(400)
        if api_parts[1] not in self.get_options:
            return self.handleCode(400)
        
        if api_parts[1] == "config":
            return self.handleGetConfig(api_parts)
        else:
            return self.handleCode(404)