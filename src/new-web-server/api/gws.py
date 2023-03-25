# Giraffe Web Server API

import os
import json

import api.api
import api.gfs
import api.ggs

class GWS(api.api.GiraffeApi):
    def __init__(self, setup_file_path):
        config = self.jsonPathToDict(setup_file_path)["api"]
        api.api.GiraffeApi.__init__(self, config)
        self.gfs = api.gfs.GFS(config)
        self.ggs = api.ggs.GGS(config)

        self.setup_file_path = setup_file_path

        self.get_options = ["status", "setup"]
        self.post_options = ["mode", "setup"]
        self.put_options = []

        self.mode_options = ["gfs", "ggs"]

        self.status = {
            "gws-status": "ok",
            "mode":"gfs",
            "gfs-connected": True,
            "ggs-connected": True,
        }

        
        
    def UpdateStatus(self):
        self.status["gfs-connected"] = self.gfs.GetStatus()
        self.status["ggs-connected"] = self.ggs.GetStatus()

    def handleGetSetup(self, path):
        if path[1] != "setup":
            return ("GWS API Not Found" + str(path), 404)
        data_file = open(self.setup_file_path, "r")
        data = json.load(data_file)
        data_file.close()
        if len(path) == 2:
            return self.DictToBytes(data)
        elif len(path) == 3 and path[2] == "web-config":
            return self.DictToBytes(data["web-config"])
        else:
            return self.handleError(404)
    
    def handlePostSetup(self, path, data):
        if path[1] != "setup":
            return self.handleError(404)
        if len(path) == 3 and path[2] == "web-config":
            expected_objects = [
                ("developer-mode", bool), ("gws-address", str), 
                ("gws-http-port", int), ("gfs-socket-address", str),
                ("gfs-socket-port", int), ("ggs-socket-address", str),
                ("ggs-socket-port", int)]
            for obj in expected_objects:
                if obj[0] not in data:
                    return self.handleError(400, "Missing " + obj[0])
                if type(data[obj[0]]) != obj[1]:
                    if obj[1] == bool:
                        if data[obj[0]] == "true":
                            data[obj[0]] = True
                        elif data[obj[0]] == "false":
                            data[obj[0]] = False
                        else:
                            return self.handleError(400, "Invalid " + obj[0])
                    try:
                        data[obj[0]] = obj[1](data[obj[0]])
                    except:
                        return self.handleError(400, "Invalid " + obj[0])
            try:
                data_file = open(self.setup_file_path, "r")
                setup_data = json.load(data_file)
                setup_data["web-config"] = data
                data_file.close()
                data_file = open(self.setup_file_path, "w")
                json.dump(setup_data, data_file, indent=4)
                data_file.close()
                return self.handleSuccess()
            except:
                return self.handleError(500, "Internal Server Error")

    def Get(self, path):
        if len(path) < 2 or path[1] not in self.get_options:
            return ("GWS API Not Found" + str(path), 404)
        
        if path[1] == "status" and len(path) == 2:          # GET /gws/status
            self.UpdateStatus()
            return self.DictToBytes(self.status)
        elif path[1] == "setup":                            # GET /gws/setup
            return self.handleGetSetup(path)
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
        if path[1] == "setup":
            return self.handlePostSetup(path, data)
        else:
            return ("GWS API Not Found" + str(path), 404)