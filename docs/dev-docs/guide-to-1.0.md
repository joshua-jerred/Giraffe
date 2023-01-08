# GFS Guide to 1.0
### Required Features for 1.0 (past 0.3)

This is just a collection of rough notes related to functionality that needs
to be implemented for a flight ready version of this software.

#### Redundancy
- [ ] GFS Watchdog Linux Service, recover from a full crash and log the crash.
- [ ] Redundant GPS/Radio?
- [ ] Power Saving

#### Flight Runner
- [ ] Receive Commands from the web server/ground station - Planned commands
  - [ ] Change flight mode
  - [ ] Change telemetry interval
  - [ ] Disable/Enable telemetry modes
  - [ ] Send specific telemetry data
  - [ ] Send errors
  - [ ] Send SSTV image
  - [ ] Enable/Disable Extensions
  - [ ] Enable/Disable Debug Modules
  - [ ] Change config file (safely)
  - [ ] Restart GPS and Radio
  
- [ ] Health Check - change flight loop
  - [ ] A health check is performed to determine the change in flight mode
  - [ ] Includes the following data:
    - [ ] gps data status
    - [ ] battery voltage
    - [ ] system status (disk space, memory, etc.)
  - [ ] Ability to restart modules if needed
  - [ ] Ability to see the status of the last shutdown/startup attempt
     - [ ] If an extension caused the flight computer to crash on startup and 
          it's not flight critical then don't restart it.

#### Extension Module
- [ ] Receive commands from the flight runner and act on those commands
- [ ] Ability to detect errors with extensions and restart them
  - [ ] If it's a flight critical extension, it needs to do whatever it can to
        get that extension back up and running

#### Extensions
 - [ ] Just shut down if there are major errors (non-flight critical)
 - [ ] Don't waste bandwidth on a shared bus (i2c)
 - [ ] BMI088 Implementation
 - [ ] Safety here is the biggest concern, extensions can not interfere with any
     other components of the flight computer
 - [ ] Battery voltage monitor implementation

#### Data Module
 - [ ] Collect specific system data like GPS, battery voltage, radio status, etc.
 - [ ] File Rotation

#### MWAV
 - [ ] APRS Implementation
 - [ ] AFSK Data Packet Implementation

#### Telemetry Module
  - [ ] Two Way Communication
  - [ ] Reduction of data packet size
    - [ ] Error enumeration
    - [ ] 5 character data codes
  - [ ] SSTV Implementation
    - [ ] Robot 12 B/W
    - [ ] Robot 36
  - [ ] APRS Implementation
  - [ ] Data packets should be in small packets, change telemetry log to be messages that are made up of multiple packets.
  - [ ] DRA818 Implementation
    - [ ] Ability to change frequency and verify that it changed
    - [ ] Detect squelch
    - [ ] RSSI
    - [ ] Redundancy!

#### Web Server
  - [ ] Commands
  - [ ] Full configuration

#### Configuration
  - [ ] Ability to change configuration file through GFS

#### Misc
 - [ ] EEPROM Implementation for GPIO configuration
 - [ ] Power on self test

#### Hardware
 - [ ] Ability to physically restart the GPS and radio hardware upon failure

####