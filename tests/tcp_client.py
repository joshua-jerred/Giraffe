# Used to connect to the GFS TCP socket.

import socket
import json
import time

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 7893  # The port used by the server

# packet = {
#   "src":"ggs",
#   "dst":"gfs",
#   "typ":"req",
#   "id":"",
#   "bdy":{
#     "rsc":"setting/general"
#   }
# }

# while True:
#     with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
#         s.connect((HOST, PORT))
#         json_msg = json.dumps(packet)
#         s.sendall(json_msg.encode("utf-8"))
#         data = s.recv(1024).decode("utf-8")
#         data = json.loads(data)["bdy"]
#     # wait
#     time.sleep(0.1)
#     print(f"Received {data!r}")

# Image Request

packet = {
  "src":"ggs",
  "dst":"gfs",
  "typ":"req",
  "id":"",
  "bdy":{
    # "rsc":"img/test_img.jpg"
    "rsc":"img/2024_02_13_145338.jpg"
  }
}

outfile = open("test_img.jpg", "wb")
BUFFER_SIZE = 512

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    json_msg = json.dumps(packet)
    s.sendall(json_msg.encode("utf-8"))

    file_size = s.recv(4)
    if len(file_size) < 4:
        print("Error: File size not received")
        exit(1)
    file_size = int.from_bytes(file_size, "little")
    print(f"Expected file size: {file_size} bytes")

    data = b""
    while len(data) < file_size:
        packet = s.recv(min(BUFFER_SIZE, file_size - len(data)))
        if not packet:
            break
        data += packet
        # print(f"Received {len(data)} bytes")

    terminate = s.recv(4)
    terminate = int.from_bytes(terminate, "little")
    print(f"Received terminate: {terminate}")

    print(f"Received file size: {len(data)} bytes")

    outfile.write(data)

    # print(len(data))
    # data = s.recv(1000)
    # print(len(data))
    # data = json.loads(data)["bdy"]
# print(f"Received {data!r}")