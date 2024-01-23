import socket
import time
import json
import sys

HOST = "127.0.0.1"
PORT = 9557

MSG = {"src": "ggs", "dst": "gdl", "typ": "req", "id": "1", "bdy": {"rsc": "status"}}

CFG = {
    "server_port": 9557,
    "logging_level": "DEBUG",
    "logging_print_to_stdout": True,
    "proactive_keep_alive": False,
    "remote_callsign": "N0CLL",
    "remote_ssid": 1,
    "source_callsign": "N0CLL",
    "source_ssid": 1,
}

INPUT_OPTIONS = {
    "s": "get status",
    "c": "get config",
    "sc": "set config",
    "l": "get log",
    "q": "quit",
}

QUIT = False

while True:
    try:
        if QUIT:
            sys.exit()
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((HOST, PORT))
        while True:
            for key, value in INPUT_OPTIONS.items():
                print(f"{key}: {value}")
            inp = input("Select option: ")
            if inp not in INPUT_OPTIONS.keys():
                print("Invalid option")
                continue
            data = ""
            new_msg = MSG.copy()
            if inp == "s":
                new_msg["bdy"]["rsc"] = "status"
            elif inp == "c":
                new_msg["bdy"]["rsc"] = "config"
            elif inp == "sc":
                new_msg["typ"] = "set"
                new_msg["bdy"]["rsc"] = "config"
                new_msg["bdy"]["dat"] = CFG
            elif inp == "l":
                new_msg["bdy"]["rsc"] = "log"
            elif inp == "q":
                QUIT = True
                s.close()
                sys.exit()
            else:
                print("Invalid option")
                continue
            s.sendall(json.dumps(new_msg).encode())
            data = s.recv(5000)
            print(f"{data!r}\n")
    except:
        if QUIT:
            sys.exit()
        print("No connection")
        time.sleep(1)
