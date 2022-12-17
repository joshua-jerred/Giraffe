# GFS Version Change Log

## 0.2.0
2021-12-16
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