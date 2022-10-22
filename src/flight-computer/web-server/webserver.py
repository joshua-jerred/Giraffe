import socket
import time
import http.server
import json
from urllib.parse import urlparse
from urllib.parse import parse_qs
import sys


class GFSData:
    def __init__(self):
        self.gfs_port = 8779
        self.host = socket.gethostname()
        self.client_socket = socket.socket()

        self.static_data = {
            "project-name": "NO-DATA",
            "main-board": "NO-DATA",
            "starting-loop": "NO-DATA",
            "console-enabled": "NO-DATA",
            "telemetry-enabled": "NO-DATA",
            "extensions": [
                {
                    "name": "NO-DATA",
                    "id": "NO-DATA",
                    "type": "NO-DATA",
                    "category": "NO-DATA",
                    "interface": "NO-DATA",
                    "interval": "NO-DATA",
                    "critical": "NO-DATA"
                }
            ]
        }

        self.dynamic_data = []

    def openConnection(self):
        try:
            self.client_socket.connect((self.host, self.gfs_port))
        except:
            print("Connection to GFS failed")
            sys.exit(1)

    def closeConnection(self):
        self.client_socket.close()

    def read(self):
        message = ""
        self.dynamic_data = []
        try:
            data = self.client_socket.recv(1024).decode()  # receive response
            if len(data) > 0:
                data = json.loads(data)
                self.parse(data)
            else:
                return {}
        except:
            print("Failed to read from GFS")
            return {}

    def write(self, message: str):
        try:
            self.client_socket.sendall(message.encode())  # send message
        except:
            print("Failed to write to GFS")

    def parse(self, data):
        if "project-name" in data:  # static data
            self.static_data["project-name"] = data["project-name"]
            self.static_data["main-board"] = data["main-board"]
            self.static_data["starting-loop"] = data["starting-loop"]
            self.static_data["console-enabled"] = data["console-enabled"]
            self.static_data["telemetry-enabled"] = data["telemetry-enabled"]
            self.static_data["extensions"] = data["extensions"]
        elif "0" in data:  # dynamic data
            for key in data:
                source = data[key]["source"]
                unit = data[key]["unit"]
                value = data[key]["value"]
                self.dynamic_data.append((source, unit, value))

    def getStaticData(self):
        return self.static_data

    def getDynamicData(self):
        return self.dynamic_data


class WebServer:
    def __init__(self):
        self.server_port = 8780
        self.host_name = "localhost"

        self.server = http.server.HTTPServer((self.host_name, self.server_port),
                                             RequestHandler)

    def start(self):
        try:
            self.server.serve_forever()
        except KeyboardInterrupt:
            print("\nShutting down...")
            sys.exit(0)


class RequestHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args):
        self.gfs = GFSData()
        self.gfs.openConnection()
        http.server.SimpleHTTPRequestHandler.__init__(self, *args)

    def do_GET(self):

        if self.path == "/":
            self.send_response(200)  # 200 = OK
            self.send_header("Content-type", "text/html")
            self.send_header("Pragma", "no-cache")
            self.end_headers()
            f = open("html/index.html", "r")
            self.wfile.write(bytes(f.read(), "utf-8"))

        elif self.path == "/favicon.ico":
            self.send_response(200)
            self.send_header("Content-type", "image/x-icon")
            self.end_headers()
            f = open("assets/favicon.ico", "rb")
            self.wfile.write(f.read())

        elif self.path == "/style.css":
            self.send_response(200)
            self.send_header("Content-type", "text/css")
            self.end_headers()
            f = open("css/style.css", "r")
            self.wfile.write(bytes(f.read(), "utf-8"))

        elif self.path == "/static-data.json":
            print("Static data requested")
            self.send_response(200)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            print("Reading static data...")
            self.gfs.read()
            print("Sending static data...")
            self.wfile.write(bytes(json.dumps(self.gfs.getStaticData()), "utf-8"))
            print("Static data sent")

        elif self.path == "/dynamic-data.json":
            self.send_response(200)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            self.gfs.read()
            self.wfile.write(bytes(json.dumps(self.gfs.getDynamicData()), "utf-8"))

        else:
            self.send_response(403)
            self.send_header("Content-type", "text/html")
            self.end_headers()


def main():
    server = WebServer()
    server.start()

if __name__ == '__main__':
    main()
