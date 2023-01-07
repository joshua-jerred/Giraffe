# GFS Guide to 1.0
### Required Features (past 0.3)

Redundancy
  - Watchdog Linux Service, recover from a crash
  - Redundant GPS/Radio?

Flight Runner
  1. Receive Commands from the web server/ground station
    - Commands include:
      - Change flight mode
      - Change telemetry interval
      - Disable/Enable telemetry modes
      - Send specific telemetry data
      - Send errors
      - Send SSTV image
      - Enable/Disable Extensions
      - Enable/Disable Debug Modules
      - Change config file (safely)
      - Restart GPS and Radio
  
  2. Health Check - change flight loop
      - A health check is performed to determine the change in flight mode
      - Includes the following data:
        - gps data status
        - battery voltage
        - system status (disk space, memory, etc.)
  3. Ability to restart modules if needed
  4. Ability to see the status of the last shutdown/startup attempt
     1. If an extension caused the flight computer to crash on startup and it's not flight critical then don't restart it.

Extension Module
  1. Receive commands from the flight runner and act on those commands
  2. Ability to detect errors with extensions and restart them
     1. If it's a flight critical extension, it needs to do whatever it can to
        get that extension back up and running

Extensions
  1. Just shut down if there are major errors (non-flight critical)
  2. Don't waste bandwidth on a shared bus (i2c)
  3. BMI088 Implementation
  4. Safety here is the biggest concern, extensions can not interfere with any
     other components of the flight computer
  5. Battery voltage monitor implementation

Data Module
  1. Collect specific system data like GPS, battery voltage, radio status, etc.

MWAV
  1. APRS Implementation
  2. AFSK Data Packet Implementation

Telemetry Module
  1. Data packets should be in small packets, change telemetry log to be messages that are made up of multiple packets.
  2. DRA818 Implementation
     1. Ability to change frequency and verify that it changed
     2. Detect squelch
     3. RSSI
     4. Redundancy!

Hardware
  1. Ability to physically restart the GPS and radio hardware upon failure