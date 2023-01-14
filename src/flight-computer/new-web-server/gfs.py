import random
import time
# Simulated Random Data

class GFS:
    def __init__(self):
        self.connected = False
        self.connection_start_time = 0

    def connect(self):
        self.connected = True
        self.connection_start_time = int(time.time())
        return self.connected

    def disconnect(self):
        self.connected = False
        return self.connected

    def getConnection(self): # Returns the connection status
        return self.connected

    def getConnectionUptime(self): # Return uptime in seconds
        if self.connected:
            return int(time.time()) - self.connection_start_time
        else:
            return 0

    ### Config
    def cfg_getConfig(self):
        return {
                'general':{
                    'gfs-version': '0.3',
                    'project-name': 'test-project',
                    'main-board-type': 'Pi Zero W 2',
                    'starting-procedure': 'testing',
                },
                    "extensions": [
                    {
                        "id": 1,
                        "name": "press1",
                        "type": "BMP180",
                        "category": "external_sensor",
                        "interface": "i2c",
                        "update-interval": 2000,
                        "flight-critical": True,
                        "i2c-bus": 1,
                        "i2c-address": "0x77"
                    },
                    {
                        "id": 2,
                        "name": "temp2",
                        "type": "DS18B20",
                        "category": "external_sensor",
                        "interface": "onewire",
                        "update-interval": 2000,
                        "flight-critical": False,
                        "onewire-id": "28-03176038beff"
                    },
                    {
                        "id": 3,
                        "name": "gps",
                        "type": "SAMM8Q",
                        "category": "gps",
                        "interface": "i2c",
                        "update-interval": 1000,
                        "flight-critical": True,
                        "i2c-bus": 1,
                        "i2c-address": "0x42"
                    },
                    {
                        "id": 4,
                        "name": "pth_sen",
                        "type": "BME280",
                        "category": "external_sensor",
                        "interface": "i2c",
                        "update-interval": 1000,
                        "flight-critical": False,
                        "i2c-bus": 1,
                        "i2c-address": "0x76"
                    },
                    {
                        "id": 5,
                        "name": "sys",
                        "type": "SYSINFO",
                        "category": "internal_sensor",
                        "interface": "internal",
                        "update-interval": 1000,
                        "flight-critical": False
                    }],
                "debugging": {
                    "console-enabled": True,
                    "console-update-interval": 1000,
                    "web-server-enabled": True,
                    "web-server-port": 8080,
                    "web-server-update-interval": 1000
                },
                "telemetry": {
                    "telemetry-enabled": True,
                    "call-sign": "FccCallSign",

                    "data-packet-mode":"aprs",

                    "aprs-enabled": True,
                    "aprs-frequency": "144.390",
                    "aprs-ssid": 11,
                    "aprs-symbol": "O",
                    "aprs-fi-pin": 1234,
                    "aprs-memo": "GFS Balloon",

                    "sstv-enabled": True,
                    "sstv-frequency": "100.000",
                    "sstv-mode": "robot12bw",

                    "afsk-enabled": True,
                    "afsk-frequency": "100.000",

                    "psk-enabled":True,
                    "psk-frequency": "100.000",
                    "psk-mode": "bpsk500"
                },

                "data-log-data-and-packet-contents": [
                    {
                        "source": "sys",
                        "unit": "load_avg_1",
                        "include-in-telemetry": True,
                        "telemetry-name": "ldav1"
                    },
                    {
                        "source": "sys",
                        "unit": "cpu_temp",
                        "include-in-telemetry": True,
                        "telemetry-name": "cpu-tp"
                    },
                    {
                        "source": "pth_sen",
                        "unit": "temp_c",
                        "include-in-telemetry": True,
                        "telemetry-name": "tc"
                    }
                    ],

                "flight-procs": [
                    {
                        "type": "testing",
                        "enabled": True,
                        "intervals": {
                            "data-log": 2,
                            "data-packet": 5,
                            "sstv": 10,
                            "aprs": 5,
                            "picture": 50,
                            "health-check": 10
                            }
                    }
                ]    
            }

    def cfg_getExtensions(self):
        return [{"name": "press1", "id": 1, "type": "BMP180", "category": 6, "interface": 3, "interval": 2000, "critical": 1, "extra-args": "BS: 1 ADDR:0x77"}, 
                {"name": "gps", "id": 3, "type": "SAMM8Q", "category": 3, "interface": 3, "interval": 1000, "critical": 1, "extra-args": "BS: 1 ADDR:0x42"}]   