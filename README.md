[![giraffe-flight-software tests](https://github.com/joshua-jerred/Giraffe/actions/workflows/gfs-tests.yml/badge.svg)](https://github.com/joshua-jerred/Giraffe/actions/workflows/gfs-tests.yml)

[![giraffe-ground-station-tests](https://github.com/joshua-jerred/Giraffe/actions/workflows/ground_station_tests.yml/badge.svg)](https://github.com/joshua-jerred/Giraffe/actions/workflows/ground_station_tests.yml)

# Giraffe <!-- omit from toc -->
### A Unified High Altitude Balloon Flight Control Framework. <!-- omit from toc -->
### [Iteration 0.4](https://giraffe.joshuajer.red/md_docs_Doxygen_pages_versions.html) 

<img src="https://user-images.githubusercontent.com/19292194/193437157-0a421f68-fa85-43bb-b372-1807a19c470d.png" height="200">

You can read about why I'm doing this here: [https://joshuajer.red/Giraffe-Introduction/](https://joshuajer.red/Giraffe-Introduction/)

Detailed development documentation can be found here: [http://giraffe.joshuajer.red/](http://giraffe.joshuajer.red/)

Check out the [project](https://github.com/users/joshua-jerred/projects/3) and 
[development branch](https://github.com/joshua-jerred/Giraffe/tree/development) 
for the latest additions.

***

## Table of Contents <!-- omit from toc -->
- [About](#about)
  - [Current High Level Functionality](#current-high-level-functionality)
  - [Planned High Level Functionality Prior to the First Flight](#planned-high-level-functionality-prior-to-the-first-flight)
  - [Development Details](#development-details)
- [Hardware](#hardware)
  - [Hat](#hat)
  - [Shoe](#shoe)
- [Interface](#interface)
    - [Web Interface](#web-interface)
    - [CLI](#cli)
- [Building and Installing the Flight Software](#building-and-installing-the-flight-software)
  - [Installing Prerequisites](#installing-prerequisites)
  - [Building the Flight Software](#building-the-flight-software)
- [Flight Software Configuration](#flight-software-configuration)
  - [Overview](#overview)
  - [General](#general)
  - [Extensions](#extensions)
    - [Extension Categories](#extension-categories)
    - [Extension Interfaces](#extension-interfaces)
    - [Extension Types](#extension-types)
      - [Currently Implemented Extensions](#currently-implemented-extensions)
      - [Simulated Extensions](#simulated-extensions)
  - [Debugging](#debugging)
  - [Telemetry](#telemetry)
    - [General](#general-1)
    - [Radios Configuration](#radios-configuration)
    - [APRS Configuration](#aprs-configuration)
    - [SSTV Configuration](#sstv-configuration)
    - [Data Packets Configuration](#data-packets-configuration)
  - [Data Log and Data Packet Contents](#data-log-and-data-packet-contents)
  - [Flight Procedures](#flight-procedures)
- [Running the Flight Software](#running-the-flight-software)
- [Other Details/Notes](#other-detailsnotes)

***

# About
Giraffe is a flight control framework for high altitude balloon (HAB) flights. 
The framework includes flight computer hardware and software, as well as 
ground station software. The purpose of this project is mainly to have a 
'learning lab' where I can experiment with all of the things that I'm interested 
in.

## Current High Level Functionality
- Detailed configuration file which allows for quick and easy setup of every 
  aspect of the flight software. (See [Flight Software Configuration](#flight-software-configuration))
- Sensors/Extensions are modular and can fail without affecting the rest of the
  flight computer. They're also threaded so that they can be continuously
  reading data without interfering with other components.
- Telemetry support for APRS, SSTV, and Data Packets.
- Aggressive error handling for all software components. Everything is designed
  to be able to fail gracefully and recover when possible.
- Web interface for flight computer debugging, configuration, and control.
- Detailed and configurable logging of data and telemetry.

## Planned High Level Functionality Prior to the First Flight
- Support for the BMI088 6-Axis IMU
- Command reception from the ground station to the flight computer
- Full configuration via the web interface
- Ground Station software using the same web interface

## Development Details
The flight software is written in C++, the web server in Python, and the web
interface is plain HTML, CSS, and JavaScript.

Being a 'learning lab' project, nearly everything is built from the ground up 
including my own implementations of:
- Linux I2C and Serial wrappers
- BCM Interface for direct GPIO control on the Raspberry Pi
- Device Drivers
   - BMP180 (I2C Environmental Sensor)
   - BME280 (I2C Environmental Sensor)
   - SAM-M8Q (I2C GPS using the UBX protocol)
   - SA868 (2w VHF Radio Module)
   - MCP3021 (I2C ADC)
- Modulation (Using my WAV file generation and Modulation library)
   - PSK - For data packets
   - AFSK, AX.25, APRS (Position and Telemetry) - For tracking via aprs.fi
   - SSTV (Robot36) - For image transmission
- A web interface for flight computer debugging, configuration, and control
- Custom hardware for the flight computer
- Simple configuration file for the flight computer

Libraries that I've written for this project:
- [WavGen, a .wav File Generator](https://github.com/joshua-jerred/WavGen)
- [MWAV, a library for modulating AFSK, AX.25, APRS, PSK31, and SSTV signals using WavGen](https://github.com/joshua-jerred/MWAV)
- [SSTV Image Tools, a library for overlaying text and getting pixel data for SSTV images](https://github.com/joshua-jerred/SSTV-Image-Tools)

Where I used existing 3rd party libraries:
- JSON parsing for the web server module and configuration reading (nlohmann/json)
- Image Manipulation for PNG/JPEG support in my SSTV Image Tools Library (Magick++)
- Google Test for unit testing

Everything is designed to be modular and extensible so that it can be used for
all types of HAB flights or other implementations. The software is made to work 
on any ARM based single board computer that runs Linux, like the Raspberry Pi
but it is targeted towards the Pi Zero 2 and Pi 4. The framework utilizes open 
communication protocols/modulation modes like APRS, AFSK, PSK31, and SSTV to 
allow for easy integration into existing software. Two way communication allows 
you to send commands to the flight computer from the ground station. 
There is a lot more included that isn't mentioned here, so please see the 
[rest of the documentation](http://giraffe.joshuajer.red/).

Too see my more older, simplified, python implementation of this 
project check out the [the AOS repo.](https://github.com/joshua-jerred/AOS)
***

# Hardware
I'm currently working on hardware prototypes.

><img src="https://user-images.githubusercontent.com/19292194/227384300-c94dd48b-5649-4411-8b46-8a4faeb5d7dc.png" height="500">
>
>Seen here: The flight computer prototype 2 PCB on top of a Pi 4. It's accompanied by the SA868 testing PCB with a dummy load antenna and it's supporting PCM2902 sound card testing PCB. Connected with debugging spaghetti.

The flight computer is (will be) made up of two boards; it is targeted towards the 
Raspberry Pi Zero 2 and maintains the same footprint. The Raspberry Pi will
be sandwiched between the two boards, with the top board being the "hat" and
the bottom board being the "shoe".

## Hat
The hat contains the majority of the circuitry. It is a four layer PCB
containing the following:
- Power Management
  - Connects to the battery (21700 Cells) via a terminal block
  - Includes a 5v linear regulator and support circuitry, this powers the Pi and 
      the other PCB
  - Voltage Divider for battery voltage measurement
- Sensors
  - BME280 (Temperature, Pressure, Humidity)
  - MCP3021 (Battery Voltage)
  - CAT24C32 (EEPROM, GPIO Configuration at boot)
  - BMI088 (6 Axis IMU)
- 2x 1-Wire Headers for DS18B20 Temperature Sensors
- 1x I2C Header for general expansion

>![image](https://user-images.githubusercontent.com/19292194/211131524-67701138-8dad-438a-b97c-4af60ae09b90.png)
>You'll notice a lot of open space on the top of the PCB, this is to isolate
the SAM-M8Q and to provide it a large ground plane. Although the ground plane
size is out of spec I have had no issues with acquiring and maintaining a GPS
lock.

### Known Issues <!-- omit in toc -->
There are a few known issues with prototype 2:
- Power Management can not reliably handle the current draw of the SA868 when 
    transmitting.
- With the voltage drop from the diode and the drop out voltage of the linear
regulator, the voltage input must be above 7.2v to maintain clean 5v output.
- The BME280 is too close to the linear regulator. This results in much higher
temperature readings and inaccurate pressure/humidity readings as they're
temperature compensated.
- The silkscreen is too small; this is the biggest issue with this prototype!

Other than that, this prototype is working great!

## Shoe
The shoe contains the telemetry circuitry. It is a four layer PCB containing
the following:
- SA868 (2w VHF Radio)
  - Isolation circuitry to prevent interference with the Pi
  - Serial Logic Level Converter (3.3v to 5v)
- Low Pass Filter and SMA Connector
- Integrated PCM2902 Audio Codes
  - USB interface with the Pi
  - 2x input channels, 2x output channels, only one of each is currently used.
- An attempt to impedance match the RF traces from the SA868 to the 
    SMA connector and provide shielding.

The radio will be on the very bottom of the hardware stack, hopefully isolating
it from the rest of the hardware.

> Front ![image](https://user-images.githubusercontent.com/19292194/227385507-7624dacc-4e9b-4813-955d-64561dc4f5c0.png)

> Back ![image](https://user-images.githubusercontent.com/19292194/227385537-62eae56d-5022-431e-9540-50a5dbabed42.png)

### Known Issues <!-- omit in toc -->
- Power control circuitry is not functional. Although 5v power is fully cut off
  with the GPIO controlled optical relay, the SA868 still draw power from
  the serial logic level converter. This results is the SA868 being unreliable
  after restarts and frequency changes.

### Shoe Testing PCBs <!-- omit in toc -->
I'm just waiting to finish debugging the SA868 and PCM2902 testing PCBs before
ordering this one.

> <p float="left">
> <img src="https://user-images.githubusercontent.com/19292194/227387219-c36bae74-d6ca-4d5a-b566-fc4badf1009e.png" height="250">
> <img src="" height="250">
> </p>
> Left: SA868 Testing PCB with a dummy load antenna. 
> 
> Right: PCM2902 Testing PCB with USB interface.


> <img src="https://user-images.githubusercontent.com/19292194/227387618-c7b300dc-241b-426e-ac53-549f9683c18d.png" height="250">
> 
>Unfortunately, I don't have the equipment to test it properly, but the SA868
>PCB includes a low pass filter to take care of the messy RF output that these
>modules are known to have.

***

# Interface
### Web Interface
The web interface is still very much a work in progress. It displays
all of the data related to configuration, extensions/modules and their statuses, and more. Commands can be send to the flight computer from this
interface. The web server is written in python using flask. It connects to the flight computer via a local socket.

![image](https://user-images.githubusercontent.com/19292194/227389769-c884f30d-89f4-478e-8bc9-0fbaf56ec118.png)

### CLI
It's currently very much out of date, but it's still functional. This
is native to the flight computer software.
![image](https://user-images.githubusercontent.com/19292194/211131919-e34518e6-2f79-4cfe-aa36-dedf77819e29.png)

***

# Building and Installing the Flight Software
## Installing Prerequisites

```bash
# For the flight software
sudo apt install build-essential cmake libi2c-dev

# For the web interface/server
sudo apt install python3 python3-pip
pip3 install flask
```

CMake will automatically download and build the following 3rd party libraries:
- Nlohmann JSON
- Google Test

## Building the Flight Software
```bash
# Clone the repo with it's submodules (MWAV, WAVGen, and SSTV Image Tools)
git clone --recurse-submodules https://github.com/joshua-jerred/Giraffe.git

# If using the --recurse-submodules flag didn't work for any reason, you can use:
# git submodule update --init --recursive --remote
# and then pull the latest change.

# Move into the repo and create a build directory
cd Giraffe && mkdir build && cd build

# Configure the build
cmake .. # You can add -DSSTV_ENABLED=OFF to disable SSTV (Requires Magick++)

# Build the flight software
make giraffe-flight-software

# Install is currently disabled until some details are finalized
# Install the flight software (Optional)
# sudo make install
```

The flight software can be found at `./Giraffe/bin/gfs` after the build.

***

# Flight Software Configuration
Some details about the configuration file are not documented here as they will
probably change in the future. 
In the tables below:
- `*` that the option is irrelevant, not fully implemented, or that implementation will change.
- `R` in the default column means that the value is required and there is no default.



The flight software is configured via a JSON file. This file is read
by the flight software at startup. It is located at 
`./Giraffe/gfs-configuration.json` in the build directory, or
`/opt/giraffe/gfs-configuration.json` after installation. The default
configuration file is located at `./Giraffe/src/flight-computer/gfs-configuration.json`.

## Overview
The configuration file is broken up into 6 sections:
- general
- extensions
- debugging
- telemetry
- flight-procedures

## General
The general section contains the following:
| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `project-name` | `string` | The name of the project. Used in telemetry. | `Giraffe` |
| `main-board-type` | `string` | `*` | `*` |
| `starting-procedure` | `string` | The name of the procedure to start with. See the flight procedure section. | `failsafe` |

### Example <!-- omit in toc -->
```json
"general": {
  "project-name": "Test Project",
  "main-board-type": "pi_zero_w_2",
  "starting-procedure": "testing"
}
```

## Extensions
Extensions are used to add sensors and other data sources to the flight 
computer.

It is represented as a JSON list of objects. Each object contains the following:
| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `id`| `int` | The ID of the extension, used to identify extension data/errors in telemetry. These must be sequential starting at 1. May be generated automatically in the future. | None, Required |
| `name` | `string` | The name of the extension. Used to identify the extension internally. Must be unique. This may be removed in the future. | None, Required |
| `type` | `string` | The type of extension. Used to identify what sensor/extension software to use. See the extension type table below. | None, Required |
| `category` | `string` | Used to identify the purpose of the extension, used internally. See the extension category table below. | None, Required |
| `interface` | `string` | The interface used to communicate with the extension. See the extension interface table below. | None, Required |
| `update-interval` | `int` | The interval in milliseconds between updates. | `5000` |
| `flight-critical` | `bool` | Whether or not the extension is critical to the flight. This changes the behavior of extension error handling. | `false` |
| `onewire-id` | `string` | The 1Wire id of the extension. Required if the interface is `onewire`. Otherwise not needed and ignored. | None, Required if `interface` is `onewire` |
| `i2c-bus` | `int` | The I2C bus number of the extension. Required if the interface is `i2c`. Otherwise not needed and ignored. | None, Required if `interface` is `i2c` |
| `i2c-address` | `string` | The I2C address of the extension in the format `0xXX`. Required if the interface is `i2c`. Otherwise not needed and ignored. | None, Required if `interface` is `i2c` |
| `spi-bus` | `int` | `*` | `*` |
| `spi-device` | `int` | `*` | `*` |
| `uart-path` | `string` | `*` | `*` |
| `uart-baud-rate` | `int` | `*` | `*` |
| `critical-source` | `string` | `*` Used for determining that this extension provides data that is used internally for flight logic. | `*` |

### Extension Categories
| Category | Description |
| -------- | ----------- |
| `gps` | A GPS unit. Tells the flight software that this extension can provide position data. |
| `camera` | A camera. Tell the flight software that this extension can provide images for SSTV. |
| `battery` | Battery data. Tell the flight software that this extension can provide battery data. |
| `system` | System data. Tell the flight software that this extension can provide system data. |
| `internal` | `*` Internal data source. The flight software will not use this extension's data for flight logic. |
| `external` | `*` External data source. The flight software will not use this extension's data for flight logic. |
| `other` | `*` |

### Extension Interfaces
| Interface | Description | Required Parameters |
| --------- | ----------- | ------------------- |
| `internal` | No interface is used. Data generated internally. | `*` |
| `i2c` | The extension uses I2C | `i2c-bus`, `i2c-address` |
| `uart` | The extension uses a UART | `uart-path` |
| `onewire` | The extension uses 1-Wire | `onewire-id` |
| `usb` | `*` The extension uses USB | `*` |
| `spi` | `*` The extension uses SPI | `*` |
| `gpio` | `*` The extension uses GPIO | `*` |

### Extension Types
#### Currently Implemented Extensions
| Type | Description | Interface | Allowed Categories |
| ---- | ----------- | ------------------- | ------------------ |
| `BMP180` | A BMP180 temperature/pressure sensor. | `i2c` | `external`, `other` |
| `BME280` | A BME280 temperature/pressure/humidity sensor. | `i2c` | `external`, `other` |
| `DS18B20` | A DS18B20 temperature sensor. | `onewire` | `external`, `other` |
| `SAMM8Q` | A SAMM8Q GPS unit, currently i2c only. | `i2c` | `gps`, `external`, `other` |
| `SYSINFO` | A system data source for linux systems. | `internal` | `system`, `internal`, `other` |
| `MCP3021` | A MCP3021 ADC. | `i2c` | `battery`, `external`, `other` |
| `RPI_CAM` | A Raspberry Pi camera. | `internal` | `camera`, `internal`|

#### Simulated Extensions
Simulated extensions can be used for testing without any external devices attached.
They generate dummy data.
| Type | Description | Interface | Allowed Categories |
| ---- | ----------- | ------------------- | ------------------ |
| `PRESS_SENSOR_SIM` | A simulated pressure sensor. | `internal` | `internal`, `external`, `other` |
| `TEMP_SENSOR_SIM` | A simulated temperature sensor. | `internal` | `internal`, `external`, `other` |
| `GPS_SIM` | A simulated GPS unit. | `internal` | `gps`, `internal`, `external`, `other` |
| `BATTERY_SENSOR_SIM` | A simulated battery voltage sensor. | `internal` | `battery`, `internal`, `external`, `other` |

### Example <!-- omit in toc -->
```json
"extensions": [
    {
        "id": 1,
        "name": "temp_sen_1",
        "type": "DS18B20",
        "category": "external_sensor",
        "interface": "onewire",
        "update-interval": 2000,
        "flight-critical": false,
        "onewire-id": "28-03176038beff"
    },
    {
        "id": 2,
        "name": "gps",
        "type": "SAMM8Q",
        "category": "gps",
        "interface": "i2c",
        "update-interval": 1000,
        "flight-critical": true,
        "i2c-bus": 1,
        "i2c-address": "0x42"
    }
]
```

## Debugging
Debugging is done via the web interface or the CLI.
| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `debug-mode-print-errors` | `bool` | Whether or not to print errors to the console. This should be disabled if CLI debugging is enabled. | `false` |
| `console-enabled` | `bool` | Whether or not to enable the CLI interface. | `false` |
| `console-update-interval` | `int` | The interval in milliseconds between CLI updates. | `1000` |
| `web-server-enabled` | `bool` | Whether or not the web server socket is enabled. | `false` |
| `web-server-socket-port` | `int` | `*` Currently the web server is setup as a file socket, this does nothing. | `*` |

### Example <!-- omit in toc -->
```json
"debugging": {
  "debug-mode-print-errors": true,
  "console-enabled": false,
  "console-update-interval": 1000,
  "web-server-enabled": true,
  "web-server-socket-port": 8080
},
```

## Telemetry
Telemetry configuration controls how the flight computer sends data to the
ground station. Currently, only SA/DRA radio modules are supported. This
means that an Amateur Radio license is required to use telemetry. It's probably
going to stay this way unless someone expresses interest in supporting other
radio modules.

### Telemetry Example <!-- omit in toc -->
See ./src/flight-computer/gfs-configuration.json for a full example.

`...` indicates that the example is shown below.
```json
"telemetry": {
  "telemetry-enabled": true,
  "callsign": "KD9GDC",
  "radios": [ ... ],
  "aprs": { ... },
  "sstv": { ... },
  "data-packets": { ... }
}
```

### General
| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `telemetry-enabled` | `bool` | Whether or not telemetry is enabled. | `false` |
| `callsign` | `string` | An FCC-registered (or equivalent) callsign. | None, required for telemetry to function. |
| `radios` | `list` | A list of radios to use for telemetry. See below. | None, required for telemetry to function. |
| `aprs` | `dict` | APRS configuration. See below. | None, required for APRS to function. |
| `sstv` | `dict` | SSTV configuration. See below. | None, required for SSTV to function. |
| `data-packets` | `dict` | PSK data packet configuration. See below. | None, required for data packets to function. |

### Radios Configuration
| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `id` | `int` | `*` The ID of the radio. This will change, see  | `*` None, required. |
| `name` | `string` | The name of the radio. | None, required. |
| `radio-type` | `string` | The type of radio. Currently only `dra-sa` is supported. | None, required. |
| `priority` | `int` | `*` The priority of the radio, starting at 1 | `*` None, required. |
| `ptt-delay` | `int` | `*` The delay in milliseconds between PTT and TX | `*` None, required |
| `frequency-ranges` | `list` | A list of frequency ranges that the radio supports. See below. | None, required. |
| `modes` | `list` | `*` The modes that the radio supports. See below. | `*` None, required. |
| `features` | `list` | `*` The features that the radio supports. See below. | `*` None, required. |
| `interface` | `dict` | The interface configuration for the radio. See below. | None, required. |

#### Radio Frequency Ranges <!-- omit in toc -->
This is a list of frequency ranges that the radio supports. Frequencies are
specified in MHz in the format `XXX.XXX`. 

##### Radio Frequency Range Example <!-- omit in toc -->
This must contain at least one range but can contain multiple.
The ranges can differ from the actual radio's capabilities, if anything it
is better to narrow the range down to the area that you will be using.
```json
"frequency-ranges": [
  {
    "min": "144.390",
    "max": "146.500"
  },
  {
    "min": "420.000",
    "max": "426.000"
  }
]
```

#### Radio Modes <!-- omit in toc -->
Has no effect in 0.4, but will be used in the future.

##### Radio Mode Example <!-- omit in toc -->
```json
"modes": [
  "TX",
  "RX",
  "APRS",
  "SSTV",
  "DATA"
]
```

#### Radio Features <!-- omit in toc -->
Has no effect in 0.4, but will be used in the future.

##### Radio Feature Example <!-- omit in toc -->
```json
"features": [
  "frequency-switching",
  "power-saving",
  "separate-tx-rx",
  "bandwidth-switching",
  "rssi",
  "volume-control",
  "squelch-control"
]
```

#### Radio Interface Configuration <!-- omit in toc -->
This will soon be overhauled. Mostly has no effect except for the GPIO
pin configuration. However, it is still enforced.
```json
"interface": {
    "type": "uart",
    "uart-port": "/dev/ttyS0",
    "uart-baud-rate": 9600,
    "gpio" : {
        "ptt":17,
        "power":6,
        "squelch-detect": 5
    }
}
```

### APRS Configuration
| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `enabled` | `bool` | Whether or not APRS is enabled. If false, all other portions of the aprs configuration are currently irrelevant. | `false` |
| `frequency` | `string` | The frequency to transmit APRS on. | None, required. |
| `ssid` | `int` | The SSID of this station, 0-15. | None, required. |
| `destination-address` | `string` | The destination address. | "APRS" |
| `destination-ssid` | `int` | The SSID of the destination address. | 0 |
| `symbol-table` | `string` | The APRS symbol table to use, either `/` or `/`. | `/` |
| `symbol` | `string` | The APRS symbol to use for this station, single character. | Dot Symbol Char |
| `telemetry-enabled` | `bool` | Whether or not to transmit telemetry data. | `false` |
| `telemetry-destination` | `string` | The destination address for telemetry data. | None, required if `telemetry-enabled` is `true`. |
| `position-enabled` | `bool` | Whether or not to transmit APRS position packets. | `false` |
| `position-comment` | `string` | The comment included in position packets. | None, required if `position-enabled` is `true`. |

#### APRS Example <!-- omit in toc -->
```json
"aprs": {
  "enabled": false,
  "frequency": "144.390",
  "ssid": 11,
  "destination-address": "APRS",
  "destination-ssid": 0,
  "symbol-table": "/",
  "symbol": "O",
  "telemetry-enabled": true,
  "telemetry-destination": "APRS",
  "position-enabled": true,
  "comment": "GFS Balloon"
}
```

### SSTV Configuration
| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `enabled` | `bool` | Whether or not SSTV is enabled. If false, all other portions of the sstv configuration are currently irrelevant. | `false` |
| `frequency` | `string` | The frequency to transmit SSTV Images on. | None, required. |
| `mode` | `string` | The SSTV mode to use. Currently, only `robot36` is supported. | `robot36` |
| `save-images` | `bool` | Whether or not to save images that were transmitted. | `false` |
| `overlay-data` | `bool` | `*` Whether or not to overlay the critical data on the image. | `*` `true` |

#### SSTV Example <!-- omit in toc -->
```json
"sstv": {
  "enabled": false,
  "frequency": "145.510",
  "mode": "robot36",
  "comment": "GFS Balloon",
  "save-images": true,
  "overlay-data": true
}
```

### Data Packets Configuration
| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `enabled` | `bool` | Whether or not data packets are enabled. If false, all other portions of the data-packets configuration are currently irrelevant. | `false` |
| `frequency` | `string` | The frequency to transmit data packets on. | None, required. |
| `mode` | `string` | The data packet mode to use. See modes below. | None, required. |
| `morse-callsign` | `bool` | Whether or not to ident in 20 wpm morse code after the packet is sent. | `true` |
| `comment` | `string` | The comment to include in the data packet. | None, required. |

#### Data Packet Modes <!-- omit in toc -->
Currently, only PSK31 style modes are supported, including:
- `bpsk125`, `bpsk250`, `bpsk500`, `bpsk1000`
- `qpsk125`, `qpsk250`, `qpsk500`

All of these are supported by fldigi.

#### Data Packet Example <!-- omit in toc -->
```json
"data-packets": {
    "enabled": true,
    "frequency": "145.510",
    "mode": "bpsk500",
    "morse-callsign": false,
    "comment": "Testing GFS PSK500 Data Packets"
}
```


## Data Log and Data Packet Contents
This is a list of all of the data that will be saved in data logs along with
the data packets. I have plans to change this very soon, but this is what it
currently looks like.

| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `source` | `string` | The name of the extension or internal generator. Refer to the dataframe in the web interface for more information. | None, Required |
| `unit` | `string` | The unit of data. | None, Required |
| `include-in-telemetry` | `bool` | Whether or not to include this data in telemetry packets. | `false` |
| `telemetry-name` | `string` | The name of the data in telemetry packets. This is helpful if you want to reduce packet size. | None, Required if `include-in-telemetry` is `true` |

### Example <!-- omit in toc -->
In this example, the load average 1 is from the extension configured with
the name `sys` and the unit is `load_avg_1`. This will now be in the
data log. It will also be included in telemetry packets with the name `ldav1`.

This shorten this line of the packet from `sys.load_avg_1=0.00` to `ldav1=0.00`.

```json
"data-log-data-and-packet-contents": [
    {
        "source": "sys",
        "unit": "load_avg_1",
        "include-in-telemetry": true,
        "telemetry-name": "ldav1"
    },
    {
        "source": "pth_sen",
        "unit": "temp_c",
        "include-in-telemetry": true,
        "telemetry-name": "tc"
    }
]
```

## Flight Procedures
Currently this is not implemented in a meaningful way. This will be used to
change flight logic based on the current flight phase (launch, ascent, descent,
recovery). Currently only the `testing` procedure is used.

`flight-procedures` is a list of procedures. These procedures contain the
following:

| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `type` | `string` | The type of procedure. Currently only `testing` is supported. | None, Required |
| `enabled` | `bool` | Whether or not this procedure is enabled at the start of the flight. | `false` |
| `intervals` | `list`| A list of intervals. See below. | None, Required |

`intervals` is a list of intervals that the flight software will use to command
actions. They can be unique to each procedure. Each value is in seconds. If
the value is `0`, the action is disabled.
It contains:
| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `data-log` | `int` | The interval in which to save data to the data log. | 0, Disabled |
| `data-packet` | `int` | The interval in which to send data packets. | 0, Disabled |
| `aprs` | `int` | The interval in which to send APRS position packets. | 0, Disabled |
| `sstv` | `int` | The interval in which to send SSTV images. | 0, Disabled |
| `picture` | `int` | `*` The interval in which to take a picture. | 0, Disabled |
| `health-check` | `int` | The interval in which to run a system check which is used to determine if the flight software should change flight procedures. | 0, Disabled |

### Example <!-- omit in toc -->
```json
"flight-procedures": [
  {
    "type": "testing",
    "enabled": true,
    "intervals": {
        "data-log": 2,
        "data-packet": 10,
        "sstv": 45,
        "aprs": 30,
        "picture": 50,
        "health-check": 10
    }
  }
]
```

***

# Running the Flight Software
After configuring the flight software, it can be started! There are two
linux service files provided in source. They are semi-functional and currently
not fully supported.

For now, the flight software can be started manually with the following:
```bash
# From the root of the project
cd bin
./gfs

# Separately, you can start the web server
cd src/web-server
python3 main.py
```

The web server will be available at `http://<ip-of-flight-computer>:8672`,
click the connect button and it should start working!


***
***
***


# Other Details/Notes

The following is from the previous README.

Currently working on hardware prototypes.
The radio/telemetry module will be on a separate board.

##### GFC Hat Prototype 1 <!-- omit in toc -->
 - Two Layer PCB with the same footprint of a Pi Zero 
 - Hardware on board:
   - BME280 - Temperature, Pressure, Humidity
   - MAX17049 - Fuel Gauge/Cell Voltage
   - EEPROM - GPIO Configuration at boot (Pi Hat Standard)
   - Linear Regulator circuit
   - I2C, 1-Wire, and SPI headers for easy expansion.
<p float="left">
<img src="https://user-images.githubusercontent.com/19292194/208227412-e66f7161-9344-4b72-9d3e-354403b7205f.png" height="250">
<img src="https://user-images.githubusercontent.com/19292194/211131143-2cf3ead8-e015-4daa-9893-1738f5f38175.png" height="250">
</p>

##### GFC Hat Prototype 2 (In progress) <!-- omit in toc -->
 - Four Layer PCB (same size)
 - Hardware on board:
   - Same BME280, EEPROM, and Linear Regulator circuit
   - BMI088 - IMU
   - Switched MAX17049 with a simple voltage divider and I2C ADC
   - SAM-M8Q - GPS Previously did not add it due to the size of the ground plane but I'm going to try as many other designs use a smaller plane. I'll compare performance to my protoboard with the recommended ground plane size.
   - 2x 1-Wire headers for the DS18B20
   - 1x I2C header for whatever!
<p float="left">
<img src="https://user-images.githubusercontent.com/19292194/211131350-2b8f4531-4663-4344-9c17-cfefae43fb73.png" height="500">

</p>

##### GFC "Shoe" Prototype 1 <!-- omit in toc -->
 - In progress, currently working with modular prototypes. This will go below the Pi Zero.
 - Embedded USB sound card and DRA818V module with LPF.

## It's an upgrade <!-- omit in toc -->
The old version lacked a lot in software, but even more in hardware. There was zero handling of EMI/RFI, it was *bad* but almost functional. The main issue was with the DRA818 module which will be handled a lot better in the future.
<img src="https://user-images.githubusercontent.com/19292194/174874402-ad9695dc-447e-425d-afe4-dd6350de5644.jpg" height="250">

There currently is no set license for this project, still trying to
decide, this will change in the future. Feel free to reference anything and if you want to use
anything please feel free to reach out!