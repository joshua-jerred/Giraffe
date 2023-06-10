# GFS Version Change Log

## 0.4
2023-03-23
The 0.4 iteration produced the first "fully functional" version of the flight
software. There was again a focus on the hardware and software for this 
iteration. Telemetry (out) is now functional including SSTV, APRS, and PSK
on separate frequencies.

### Software
- Telemetry
  - Implemented AX.25 protocol alongside APRS in MWAV
    - Added APRS position packets in the telemetry module
  - Implemented SSTV in MWAV and added to the telemetry module
  - Created a simple image tools library for image manipulation (callsign)
  - Logic for radio control including frequency switching and future flexibility
      for multiple radios.
- MCP3021 ADC driver
- Better handling of GPS data along with data anomaly detection
- SA868 driver (flexible for other DRA/SA modules)
- BCM GPIO implementation that will be used across the threads
- Web server overhaul
  - Switched to using Flask
  - More reliable but still needs work
  - Completely different site structure and design
  - Basic linux service implementation
- Large server module changes to support the new web server
- Large structure changes to the project
  - CMake overhaul
  - Submoduled MWAV, WavGen, and SSTV Image Tools
- Various bug fixes and small improvements

### Hardware
The main flight computer PCB (Hat) was tested and debugged along side the 
prototype boards that will be made into the telemetry PCB (Shoe). The Hat
is mostly functional but has major issues with it's power supply. The Shoe
prototype board was also nearly fully functional but had issues with it's
power switching circuit and the squelch detection circuit. The sound card
is fully functional but may require better filtering.



## 0.3
2023-01-06
 - This iteration focused on working with the actual GFC hardware for the first time,
    lots of PCB work on this one. Used prototype 1 of the hat, developed prototype 2.
 - Figured out that I never enabled the correct warnings for the compiler, so that
     was changed and it resulted in a wall of errors, which were fixed.
 - Targeting the Pi Zero W 2 instead of the Zero W as it is not getting the support it needs anymore (as expected).
 - Worked on the I2C utility, transactions are now thread safe and there is more functionality.
 - Added unit test 6 through 11
 - Completed a large refactor of modules and extensions, putting them in a namespace.
 - Added initial UBX DDC driver - Fighting with unreliable I2C transactions on the Pi.
 - Added BME280 driver/extension
 - Changed telemetry to now use 'MWAV', my other project, for the modulation.
 - Added a 'command' infrastructure to the data stream, flight runner, and web server to allow
      for remote control of the flight computer. This will eventually be added to the telemetry module.
      This will allow for *a lot* of future functionality.
 - Configuration for what data is logged and transmitted is now done in the config file.
 - Added a 'telemetry log' to the telemetry module. This allows for retransmission of packets upon request.
   - Telemetry log is stored as text in ram, not audio, saving a lot of space.
 - Worked on the Web Server, adding the telemetry log and basic command functionality. Data packets can be requested again from the web server and then will be added to the telemetry queue. The web server also displays errors.
 - Added the SAM-M8Q Extension.
 - Added a 'system info' extension.
 - Lots of work on documentation.
 - Finalized the future extension list.

## 0.2.0
2022-12-16
 - Overall spent a lot of time working on fixing small bugs to provide a clear path for future updates
 - Added I2C utility
 - Added OneWire utility
 - Added PSK Modulation
 - Added 'Data Packet' implementation for PSK
 - Started work on AFSK modulation - not implemented into GFS yet
 - Improved implementation of the web server
   - Fixed many bugs related to connecting/disconnecting/shutting down
 - Completed base implementation of the telemetry module
   - TX Queue is fully working
 - Added DS18B20 Extension
 - Added BMP180 Extension
 - Hardware Prototypes
   - Started prototyping 'GFC'
   - Decided on base hardware
 - Implemented 'extra-args' for extensions

## 0.1.0
2022-10-31
 - Error Frame
 - Server Module
 - Python web server
 - Basic telemetry module
 - Data Socket

## 0.0.9
2022-10-10
 - Added console module for logging and debugging.
 - Added more simulated sensors.
 - Added outline and most basic implementation of the telemetry module.
 - Implemented the 'data frame' concept within the data module.
 - Added Doxygen documentation.
 - Checks for 'stale data' in the data module / data frame.
 - Added basic data logging.

0.0.1 - 2022-08-30
 - It runs, basic proof of concept of threaded design.
 - Design/Early Prototypes of the C++ Version Started 7/1/2022