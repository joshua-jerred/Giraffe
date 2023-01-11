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
    def cfg_getGeneral(self):
        return {
            'gfs-version': '0.3',
            'project-name': 'test-project',
            'main-board': 'Pi Zero W 2'
        }

    def cfg_getDebug(self):
        return {
            "console-enabled": True,
            "console-update-interval": 1000,
            "console-log-level": "debug",
            "web-server-enabled": True,
            "web-server-update-interval": 1000
        }

    def cfg_getFlightProcedures(self):
        return [{"type": "testing", "enabled": True,
                "intervals": {
                    "data-log": 2,
                    "data-packet": 5,
                    "sstv": 10,
                    "aprs": 5,
                    "picture": 50,
                    "health-check": 10}},
                {"type": "ascent", "enabled": False,
                "intervals": {
                    "data-log": 1243,
                    "data-packet": 5832,
                    "sstv": 1091,
                    "aprs": 12,
                    "picture": 62,
                    "health-check": 64}}]

    def cfg_getExtensions(self):
        return [{"name": "press1", "id": 1, "type": "BMP180", "category": 6, "interface": 3, "interval": 2000, "critical": 1, "extra-args": "BS: 1 ADDR:0x77"}, 
                {"name": "gps", "id": 3, "type": "SAMM8Q", "category": 3, "interface": 3, "interval": 1000, "critical": 1, "extra-args": "BS: 1 ADDR:0x42"}]

    def cfg_getTelemetry(self):
        return {
        "telemetry-enabled": True,
        "callsign": "FccCallSign",
        
        "data-packet-mode":"afsk",
        "data-packet-frequency":"100.000",

        "aprs-enabled": True,
        "aprs-frequency": "144.390",
        "aprs-ssid": 11,
        "aprs-symbol": "O",
        "aprs-memo": "GFS Balloon",

        "sstv-enabled": True,
        "sstv-frequency": "100.000",

        "afsk-enabled": True,
        "afsk-frequency": "100.000",

        "psk-enabled":True,
        "psk-frequency": "100.000",
        "psk-mode": "bpsk500"}    

    def cfg_getDataPacketContents(self):
        return [
        {
            "source": "sys",
            "unit": "load_avg_1",
            "telemetry-name": "ldav1"
        },
        {
            "source": "sys",
            "unit": "cpu_temp",
            "telemetry-name": "cpu-tp"
        },
        {
            "source": "pth_sen",
            "unit": "temp_c",
            "telemetry-name": "tc"
        }]
    
    def cfg_getDataLogContents(self):
        return [
        {
            "source": "sys",
            "unit": "load_avg_1",
        },
        {
            "source": "sys",
            "unit": "cpu_temp",
        },
        {
            "source": "pth_sen",
            "unit": "temp_c",
        }]
