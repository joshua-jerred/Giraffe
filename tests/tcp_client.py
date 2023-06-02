# Used to connect to the GFS TCP socket.

import socket
import json

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 7893  # The port used by the server

packet = {
  "src":"ggs",
  "dst":"gfs",
  "typ":"req",
  "cat":"data",
  "id":"",
  "body":{
    "section":"system_info"
  }
}

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    json_msg = json.dumps(packet)
    s.sendall(json_msg.encode("utf-8"))
    data = s.recv(1024)

print(f"Received {data!r}")
