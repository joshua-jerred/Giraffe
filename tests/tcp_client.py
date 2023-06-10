# Used to connect to the GFS TCP socket.

import socket
import json
import time

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 7893  # The port used by the server

packet = {
  "src":"ggs",
  "dst":"gfs",
  "typ":"req",
  "id":"",
  "body":{
    "rsc":"data/server_module_stats"
  }
}

while True:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        json_msg = json.dumps(packet)
        s.sendall(json_msg.encode("utf-8"))
        data = s.recv(1024).decode("utf-8")
        data = json.loads(data)["body"]
    # wait
    time.sleep(0.1)
    print(f"Received {data!r}")

