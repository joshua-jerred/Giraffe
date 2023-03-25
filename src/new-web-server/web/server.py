import json
from http.server import BaseHTTPRequestHandler, HTTPServer
from functools import partial

from web.files import Files
import api.gws

SETUP_FILE = "data/setup.json"
CORS_POLICY = "*"

class RequestHandler(BaseHTTPRequestHandler):
    def __init__(self, gws_api, *args, **kwargs):
        self.gws = gws_api

        setup_json = {}
        with open(SETUP_FILE, "r") as f:
            setup_json = json.load(f)

        self.files = Files(setup_json["pages"])

        BaseHTTPRequestHandler.__init__(self, *args, **kwargs)

    def end_headers(self):
        self.send_header("Access-Control-Allow-Origin", CORS_POLICY)
        return super().end_headers()

    def handleResponseCode(self, code, message=""):
        if message == "":
            message = "Response Code {}".format(code)
        self.send_response(code)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        self.wfile.write(bytes(message, "utf-8"))

    def handleGetStatic(self, file=""):
        content = ""
        content_type = ""
        if "." in file:
            content, content_type = self.files.getFile(file)
        else:
            content, content_type = self.files.getPage(file)

        if type(content_type) == int:
            self.handleResponseCode(content_type, content)
            return
        
        self.send_response(200)
        self.send_header("Content-type", content_type)
        self.end_headers()
        self.wfile.write(content)

    def handleGetAPI(self, api_parts):
        content = ""
        content_type = ""
        if api_parts[0] == "gws":
            content, content_type = self.gws.Get(api_parts)    
        elif api_parts[0] == "gfs":
            content, content_type = self.gws.gfs.Get(api_parts)
            #self.handleResponseCode(501, "Not implemented")
        elif api_parts[0] == "ggs":
            #content, content_type = self.ggs.Get(api_parts)
            self.handleResponseCode(501, "Not implemented")
        else:
            self.handleResponseCode(404, "API not found" + str(api_parts))
            return
        
        if type(content_type) == int:
            self.handleResponseCode(content_type, content)
            return
        
        self.send_response(200)
        self.send_header("Content-type", content_type)
        self.end_headers()
        self.wfile.write(content)
        
    def handlePostAPI(self, api_parts):
        content = ""
        response_code = ""

        if self.headers["content-type"] != "application/json":
            self.handleResponseCode(400, "Content-Type must be application/json")
            return

        length = int(self.headers["Content-Length"])
        data = self.rfile.read(length)
        data = json.loads(data.decode("utf-8"))

        if api_parts[0] == "gws":
            content, response_code = self.gws.Post(api_parts, data)
        elif api_parts[0] == "gfs":
            #content, content_type = self.gfs.Post(api_parts)
            self.handleResponseCode(501, "Not implemented")
        elif api_parts[0] == "ggs":
            #content, content_type = self.ggs.Post(api_parts)
            self.handleResponseCode(501, "Not implemented")
        else:
            self.handleResponseCode(404, "API not found" + str(api_parts))
            return
        self.handleResponseCode(response_code, content)
        
    def do_GET(self):
        path_parts = self.path.split("/")
        if self.path == "/" or "." in self.path:
            self.handleGetStatic("" if self.path == "/" else self.path[1:])
        else:
            path_parts = self.path.split("/")
            if len(path_parts) == 2:
                self.handleGetStatic(path_parts[1])
            elif len(path_parts) >= 3:
                self.handleGetAPI(path_parts[1:])
            else:
                self.handleResponseCode(404)

    def do_POST(self):
        path_parts = self.path.split("/")
        if len(path_parts) >= 3:
            self.handlePostAPI(path_parts[1:])
        else:
            self.handleResponseCode(404)

def start():
    FALLBACK_ADDRESS = "localhost"
    FALLBACK_PORT = 7890

    setup = {}
    with open(SETUP_FILE, "r") as f:
        setup = json.load(f)["web-config"]

    address = setup["gws-address"]
    port = setup["gws-http-port"]
    print("Starting server at http://{}:{}/".format(address, port))
    
    gws_api = api.gws.GWS(SETUP_FILE)

    request_handler = partial(RequestHandler, gws_api)
    try:
        server = HTTPServer((address, port), request_handler)
        server.serve_forever()
    except:
        print("Failed to start server at http://{}:{}/".format(address, port))
        print("Starting server at http://{}:{}/".format(FALLBACK_ADDRESS, FALLBACK_PORT))
        server = HTTPServer((FALLBACK_ADDRESS, FALLBACK_PORT), request_handler)
        server.serve_forever()