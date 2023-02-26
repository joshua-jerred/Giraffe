import time
import json
import socket

class GiraffeWebServer:
    def __init__(self):
        self.gfs_port = 8779

        self.connection_requested = False
        self.connected = False
        self.connection_start_time = 0

        self.host = socket.gethostname()
        self.client_socket = socket.socket()

    def connect(self):
        self.connection_requested = True
        try:
            self.client_socket.connect((self.host, self.gfs_port))
            self.connected = True
            self.connection_start_time = int(time.time())
            return True
        except Exception as e:
            print("Connection to GFS failed")
            self.connected = False
            self.__init__()
            self.connection_requested = True
            return False

    def disconnect(self):
        self.connection_requested = False
        self.client_socket.close()
        self.connected = False

    def getConnectionRequested(self):
        return self.connection_requested

    def getConnectionStatus(self): # Returns the connection status
        return self.connected

    def getConnectionUptime(self): # Return uptime in seconds
        if self.connected:
            return int(time.time()) - self.connection_start_time
        else:
            return 0

    def get(self, key: str):
        command = "get-" + key
        if (self._write(command)):
            return self._read()

    def command(self, command: str):
        if (self._write(command)):
            return self._read()

    def set(key: str, value: str):
        pass

    def _read(self):
        try:
            data = self.client_socket.recv(5000).decode()  # receive response
            if len(data) > 0:
                return json.loads(data)
            else:
                return {}
        except Exception as e: 
            print(e)
            print("Failed to read from GFS")
            return {}

    def _write(self, message: str):
        try:
            self.client_socket.sendall(message.encode())  # send message
            return True
        except:
            print("Failed to write to GFS - reconnecting")
            self.connected = False
            connection_requested = self.connection_requested
            self.__init__()
            self.connection_requested = connection_requested
            return False