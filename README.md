[![Giraffe Unit Tests](https://github.com/joshua-jerred/Giraffe/actions/workflows/giraffe_ci.yml/badge.svg)](https://github.com/joshua-jerred/Giraffe/actions/workflows/giraffe_ci.yml)

# Giraffe <!-- omit from toc -->
### A Unified Flight Command and Control System <!-- omit from toc -->
### [Iteration 0.5](https://giraffe.joshuajer.red/md_docs_Doxygen_pages_versions.html)

***
## Learn More About it
Read about why I'm doing this: [https://joshuajer.red/Giraffe-Introduction/](https://joshuajer.red/Giraffe-Introduction/)

Detailed software documentation (Doxygen): [http://giraffe.joshuajer.red/](http://giraffe.joshuajer.red/)

Check out the [Giraffe GitHub Project](https://github.com/users/joshua-jerred/projects/3) to see what I'm currently working on.

***

## Photos
>![Giraffe Network Diagram](https://github.com/joshua-jerred/Boost_erSeat/assets/19292194/ad96b114-5cb1-4863-8c5f-f746747301ee)
> The Giraffe Network - A diagram of Giraffe's components and how they interact with each other.

><img src="https://user-images.githubusercontent.com/19292194/227384300-c94dd48b-5649-4411-8b46-8a4faeb5d7dc.png" height="500">
>
> The flight computer prototype 2 PCB on top of a Pi 4. It's accompanied by the SA868 testing PCB with a dummy load antenna and it's supporting PCM2902 sound card testing PCB. Connected with debugging spaghetti.


>![The Data Monitor Page from GWC](https://github.com/joshua-jerred/Giraffe/assets/19292194/916d2c24-f5f0-4ec3-a7f1-9a830c0bef41)
>![The Configuration Page from GWC](https://github.com/joshua-jerred/Giraffe/assets/19292194/143f6b7d-5324-4dfa-b378-d6788fffb702)
> This is the web client, used to configure the flight computer on the ground
> and to monitor/control it in the air.

***

## Table of Contents <!-- omit from toc -->
- [About](#about)
  - [Current High Level Functionality](#current-high-level-functionality)
  - [Planned High Level Functionality Prior to the First Flight](#planned-high-level-functionality-prior-to-the-first-flight)
  - [Development Details](#development-details)
- [Hardware](#hardware)
  - [Hat](#hat)
  - [Shoe](#shoe)
- [Building and Installing the Flight Software](#building-and-installing-the-flight-software)
  - [Installing Prerequisites](#installing-prerequisites)
  - [Building the Flight Software](#building-the-flight-software)
- [Flight Software Configuration/Web Client](#flight-software-configurationweb-client)

***

# About
Giraffe is a "flight command and control system" first developed for high 
altitude balloon flights.

The purpose of this project is mainly to have a 'learning lab' where I can experiment with all of the things that I'm interested in. This includes C++, Linux Embedded Development, Circuit Design, RF/Amateur Radio, Node.js, CI, interfacing hardware with the web, meta-programming, and more.

The system includes the following key components:
- **GFS** Giraffe Flight System
  - The hardware and software that actually flies
- **GGS** Giraffe Ground Station
  - Software that helps interface with the flight system both on the ground
    and in the air.
- **GDL** Giraffe Data Link
  - The hardware and software used on both GFS and GGS to connect them together
    via long distance radio links.
- **GWC** Giraffe Web Client
  - A web client to easily configure, control, and monitor all Giraffe 
    components in one place.

## Current High Level Functionality
- Detailed configuration via the web interface of all system components.
- Telemetry support for APRS, SSTV, and Data Packets.
- Aggressive error handling for all software components. Everything is designed
  to be able to fail gracefully and recover when possible.
- Detailed and configurable logging of data and telemetry.
- Drivers/Firmware for the Raspberry Pi BCM interface along with many different sensors, radios, and other hardware.

## Planned High Level Functionality Prior to the First Flight
- Two way communication via GDL

## Development Details
The flight software is written in C++, the web interface and ground stations are written in Node.js, and some Python is sprinkled in.

There are common components, including a large collection of configuration
and data point metadata files that are used to generate code for both the 
flight system and the ground station. For example, to add a configuration
option for the flight computer, you just need to add the information to the
metadata file, run the generator, rebuild, and redeploy. It will then be inside
of the flight software, configurable via the web interface, and safely 
implemented to restrained values to prevent misconfiguration.
This works for integers, floats, strings, and enums.

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
- [Boost_erSeat, a library with a few useful functions that I use in many different projects](https://github.com/joshua-jerred/Boost_erSeat)
- [WavGen, a .wav File Generator](https://github.com/joshua-jerred/WavGen)
- [MWAV, a library for modulating AFSK, AX.25, APRS, PSK31, and SSTV signals using WavGen](https://github.com/joshua-jerred/MWAV)
- [SSTV Image Tools, a library for overlaying text and getting pixel data for SSTV images](https://github.com/joshua-jerred/SSTV-Image-Tools)

Where I used existing 3rd party libraries:
- JSON parsing for the web server module and configuration reading (nlohmann/json)
- Image Manipulation for PNG/JPEG support in my SSTV Image Tools Library (Magick++)
- Google Test for unit testing

Everything is designed to be modular and extensible so that it can be used for
all types of HAB flights or even some other future implementations where command 
and control is needed. The software is made to work  on any ARM based single 
board computer that runs Linux, like the Raspberry Pi but it is targeted towards 
the Pi Zero 2 and Pi 4. 

The framework utilizes open communication protocols/modulation modes like APRS, 
AFSK, PSK31, and SSTV to allow for easy integration into existing software. Two 
way communication, which is currently in development, allows you to send 
commands to the flight computer from the ground station. There is a lot more 
included that isn't mentioned here, so please see the 
[rest of the documentation](http://giraffe.joshuajer.red/).

Too see my more older, simplified, python implementation of this 
project check out the [the AOS repo.](https://github.com/joshua-jerred/AOS)
***

# Hardware
I'm currently working on hardware prototypes.

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
- Power Management! I'm currently working on replacing the linear regulator with
  a boost converter.
- The BME280 is too close to the power supply. This results in much higher
temperature readings and inaccurate pressure/humidity readings as they're
temperature compensated.

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
  after restarts and frequency changes. More isolation needs to be added.

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

# Building and Installing the Flight Software
## Installing Prerequisites


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

# Build the flight system
make giraffe_flight_system

# Install is currently disabled until some details are finalized
# Install the flight system (Optional)
# sudo make install
```

***

# Flight Software Configuration/Web Client

Configuration is no longer done through the config file, but instead through
the web interface! This is where the ground station and flight system are
configured.

Multiple clients can be connected to the web interface at once to allow for a
team to monitor the flight together.

![The Configuration Page from GWC](https://github.com/joshua-jerred/Giraffe/assets/19292194/143f6b7d-5324-4dfa-b378-d6788fffb702)

***

Development is no longer done by release, but instead features will be added
as they are completed.