from multiprocessing import Process
import socket
import json
import time
import pathlib

ADDRESS = 'localhost'
PORT = 7893
MAX_MSG_LEN = 1024
SOCKET_TIMEOUT = 1

PROTOCOL_SRC = "ggs"
PROTOCOL_DST = "gfs"

SCRIPT_PATH = pathlib.Path(__file__).parent.resolve()
GFS_RESOURCES_PATH = SCRIPT_PATH.parent.parent / "project" / "metadata" / "gfs_resources.json"
RESOURCES = json.load(open(GFS_RESOURCES_PATH))

class GfsConnection:
    def __init__(self, host, port):
        self.host = host
        self.port = port

    def __connect(self):
        self.sock = socket.socket(
            socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((self.host, self.port))
        self.sock.settimeout(SOCKET_TIMEOUT)

    def __disconnect(self):
        self.sock.close()

    def getData(self, category):
        res = self.__getItem("data/" + category)
        if res is not None and res["typ"] == "rsp" and res["bdy"]["cde"] == "ok":
            return res["bdy"]["dat"]
        else:
            return None

    def getSetting(self, category):
        return self.__getItem("setting/" + category)

    def __exchange(self, json_data):
        msg = json.dumps(json_data).encode()
        
        self.__connect()
        sent = self.sock.send(msg)
        if sent == 0:
            raise RuntimeError("socket connection broken while sending")
        received = self.sock.recv(MAX_MSG_LEN)
        self.__disconnect()
        
        if received == 0:
            raise RuntimeError("socket connection broken while receiving")
        try:
            return json.loads(received.decode())
        except:
            return None

    def __getItem(self, rsc):
        packet = self.__createPacket("req", {"rsc" : rsc})
        return self.__exchange(packet)

    def __createPacket(self, type:str, body:dict):
        return {"src" : PROTOCOL_SRC, "dst" : PROTOCOL_DST, "typ": type, "id" : "1", "bdy" : body}

class DataItem:
    def __init__(self, category):
        self.category = category
        self.last_updated = time.time()
        self.valid = False
        self.data = {}

    def getSecondsSinceLastUpdate(self):
        return time.time() - self.last_updated

    def update(self, data, valid=True):
        self.last_updated = time.time()
        self.valid = valid
        self.data = data

    def __repr__(self) -> str:
        return f"{self.category} : last updated {self.getSecondsSinceLastUpdate()}, valid {self.valid}\n{self.data}"

class GfsSync:
    def __init__(self):
        self.data_categories = []
        for category in RESOURCES:
            self.data_categories.append(category)
        self.data_items = {}
        for category in self.data_categories:
            self.data_items[category] = DataItem(category)

    def __updateDataItem(self, category):
        con = GfsConnection(ADDRESS, PORT)
        data = con.getData(category)
        if data is not None:
            self.data_items[category].update(data)

    def update(self):
        for category in self.data_categories:
            self.__updateDataItem(category)

def main():
    s = GfsSync()
    s.update()

if __name__ == '__main__':
    main()