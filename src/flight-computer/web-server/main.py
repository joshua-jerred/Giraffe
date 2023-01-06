"""
@file main.py
@author Joshua Jerred (github.com/joshua-jerred)
@brief HTTP server/Socket client for GFS
@details 
This file contains the HTTP server and socket client for the GFS.
It requests json data from GFS upon a GET request.
For example, once started if a user navigates to http://localhost:8780/telemetry.json
the server will request the telemetry data from GFS and return it as a json file.

It's a bit messy at the moment and could use some refactoring, but it works and
does not interfere with GFS upon any known failure.
"""

import socket
import time
import http.server
import json
from urllib.parse import urlparse
from urllib.parse import parse_qs
import sys
import functools

class GFSData:
    def __init__(self):
        self.gfs_port = 8779
        self.host = socket.gethostname()
        self.client_socket = socket.socket()

        self.static_data = {
            "project-name": "NO-DATA",
            "main-board": "NO-DATA",
            "starting-proc": "NO-DATA",
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

        self.dynamic_data = {}
        self.telemetry_data = {}
        self.openConnection()

    def openConnection(self):
        try:
            self.client_socket.connect((self.host, self.gfs_port))
        except:
            print("Connection to GFS failed")

    def closeConnection(self):
        self.client_socket.close()

    def read(self):
        self.dynamic_data = {}
        self.telemetry_data = {}
        try:
            data = self.client_socket.recv(5000).decode()  # receive response
            if len(data) > 0:
                data = json.loads(data)
                self.parse(data)
            else:
                return {}
        except Exception as e: 
            print(e)
            print("Failed to read from GFS")
            return {}

    def write(self, message: str):
        try:
            self.client_socket.sendall(message.encode())  # send message
            return 0
        except:
            print("Failed to write to GFS - reconnecting")
            self.__init__()
            return -1

    def parse(self, data):
        if "project-name" in data:  # static data
            self.static_data["project-name"] = data["project-name"]
            self.static_data["main-board"] = data["main-board"]
            self.static_data["starting-proc"] = data["starting-proc"]
            self.static_data["console-enabled"] = data["console-enabled"]
            self.static_data["telemetry-enabled"] = data["telemetry-enabled"]
            self.static_data["extensions"] = data["extensions"]
        elif "dynamic" in data:  # dynamic data
            self.dynamic_data = data
            #for key in data["dynamic"]:
            #    source = data[key]["source"]
            #    unit = data[key]["unit"]
            #    value = data[key]["value"]
            #    self.dynamic_data[key] = (source, unit, value)
        elif "tx-log" in data:  # telemetry data
            self.telemetry_data = data
        else:
            print("Unknown data type")

    def getStaticData(self):
        if (self.write("static")) == 0:
            self.read()
        return self.static_data

    def getDynamicData(self):
        if (self.write("dynamic")) == 0:
            self.read()
        else:
            return {"dynamic": "failed"}
        return self.dynamic_data

    def getTelemetryData(self):
        if (self.write("telemetry")) == 0:
            self.read()
        else:
            return {"telemetry": "failed"}
        return self.telemetry_data


class WebServer:
    def __init__(self):
        self.server_port = 8780
        self.host_name = "0.0.0.0" # Accessible from LAN, otherwise use localhost for local access only

        self.gfs = GFSData()
        
        
        handler = functools.partial(RequestHandler, self.gfs)

        self.server = http.server.HTTPServer((self.host_name, self.server_port),
                                             handler)

    def start(self):
        try:
            self.server.serve_forever()
        except KeyboardInterrupt:
            print("\nShutting down...")
            sys.exit(0)

class RequestHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, GFS, *args, **kwargs):
        self.gfs = GFS
        super().__init__(*args, **kwargs)

    def log_message(self, format, *args): # disable logging
        return

    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        command = json.loads(post_data)["command"]
        self.send_response(200)
        self.end_headers()
        if command == "shutdownServer":
            self.gfs.write("shutdownServer")
            self.gfs.closeConnection()
            print("Shutdown Requested - Shutting down...")
            sys.exit(0)
        elif command == "shutdownGFS":
            self.gfs.write("shutdownGFS")
            print("Shutdown Requested - Shutting down...")
            sys.exit(0)
        elif command == "DISCONNECT":
            self.gfs.write("DISCONNECT")
        elif command[0:4] == "cmd/":
            print("Requesting retransmission of packet " + command[12:])
            self.gfs.write(command)
        else:
            print("Unknown command: " + command)

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
            self.send_response(200)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            self.wfile.write(bytes(json.dumps(self.gfs.getStaticData()), "utf-8"))

        elif self.path == "/dynamic-data.json":
            self.send_response(200)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            self.wfile.write(bytes(json.dumps(self.gfs.getDynamicData()), "utf-8"))

        elif self.path == "/telemetry-data.json":
            self.send_response(200)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            self.wfile.write(bytes(json.dumps(self.gfs.getTelemetryData()), "utf-8"))

        elif self.path == "/data-display.js":
            self.send_response(200)
            self.send_header("Content-type", "application/javascript")
            self.end_headers()
            f = open("js/data-display.js", "r")
            self.wfile.write(bytes(f.read(), "utf-8"))

        else:
            self.send_response(403)
            self.send_header("Content-type", "text/html")
            self.end_headers()


def main():
    server = WebServer()
    print("Starting server on port: " + str(server.server_port))
    server.start()

    #gfs = GFSData()
    #gfs.openConnection()
    #print(gfs.getStaticData())
    #print(gfs.getDynamicData())

if __name__ == '__main__':
    main()
