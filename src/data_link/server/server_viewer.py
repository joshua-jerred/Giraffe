import socket
import time
import json
import sys

HOST = "127.0.0.1"
PORT = 9557

MSG = {"src": "ggs", "dst": "gdl", "typ": "req", "id": "1", "bdy": {"rsc": "status"}}

CFG = {
    "logging_level": "DEBUG",
    "logging_print_to_stdout": False,
    "proactive_keep_alive": False,
    "remote_callsign": "NOCALL",
    "remote_ssid": 0,
    "source_callsign": "NOCALL",
    "source_ssid": 0,
}

INPUT_OPTIONS = {"s": "get status", "c": "get config", "q": "quit", "sc": "set config"}

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
while True:
    try:
        s.connect((HOST, PORT))
        while True:
            for key, value in INPUT_OPTIONS.items():
                print(f"{key}: {value}")
            inp = input("Select option: ")
            if inp not in INPUT_OPTIONS.keys():
                print("Invalid option")
                continue

            new_msg = MSG.copy()
            if inp == "s":
                new_msg["bdy"]["rsc"] = "status"
            elif inp == "c":
                new_msg["bdy"]["rsc"] = "config"
            elif inp == "q":
                s.close()
                exit()
            else:
                print("Invalid option")
                continue
            s.sendall(json.dumps(MSG).encode())
            data = s.recv(1000)
            print(f"{data!r}\n")
    except:
        print("No connection")
        time.sleep(1)
