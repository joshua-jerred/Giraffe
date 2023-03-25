# Giraffe Web Server API

import os
import json

import api.gfs
import api.ggs

class GWS:
    def __init__(self, setup_file_path, encoding="utf-8"):
        self.gfs = api.gfs.GFS(setup_file_path)
        self.ggs = api.ggs.GGS(setup_file_path)

        self.encoding = encoding
        self.setup_file_path = setup_file_path

        self.get_options = ["status", "setup"]
        self.post_options = ["mode"]
        self.put_options = []

        self.mode_options = ["gfs", "ggs"]

        self.status = {
            "gws-status": "ok",
            "mode":"gfs",
            "gfs-connected": True,
            "ggs-connected": True,
        }
        
    def DictToBytes(self, dict):
        content_type = "application/json"
        return (bytes(json.dumps(dict), self.encoding), content_type)

    def UpdateStatus(self):
        self.status["gfs-connected"] = self.gfs.GetStatus()
        self.status["ggs-connected"] = self.ggs.GetStatus()

    def Get(self, path):
        if len(path) < 2 or path[1] not in self.get_options:
            return ("GWS API Not Found" + str(path), 404)
        
        if path[1] == "status" and len(path) == 2:          # GET /gws/status
            self.UpdateStatus()
            return self.DictToBytes(self.status)
        elif path[1] == "setup" and len(path) == 2:         # GET /gws/setup
            with open(self.setup_file_path) as f:
                setup = json.load(f)
                return self.DictToBytes(setup)
        else:
            return ("GWS API Not Found" + str(path), 404)    
    
    def Post(self, path, data):
        if len(path) < 2 or path[1] not in self.post_options:
            return ("GWS API Not Found" + str(path), 404)
        
        if path[1] == "mode" and len(path) == 2:
            if "mode" in data:
                if data["mode"] in self.mode_options:
                    print("Mode set to " + data["mode"])
                    self.status["mode"] = data["mode"]
                    return ("Mode set to " + data["mode"], 200)
                else:
                    return ("Mode not valid", 400)
            else:
                return ("Mode not specified", 400)
        else:
            return ("GWS API Not Found" + str(path), 404)