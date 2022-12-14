# helpful tools for hardware debugging:
- ``i2cdetect -y 1`` (detects i2c devices)
- A $10 logic analyzer and the Saleae Logic software
- https://github.com/sparkfun/SparkFun_u-blox_UBX_HLA
   This tool by sparkfun is *amazing* when debugging the GPS with the Saleae 
   Logic software. It doesn't seem to get much love, but I think it's the
   greatest time saver for debugging u-blox GPS modules.

# sync:
From Giraffe parent:
rsync -v -r Giraffe/src/ balloon:Giraffe/src
rsync -v -r Giraffe/tests/ balloon:Giraffe/tests

# prereqs
 - sudo apt-get install libi2c-dev


# Errors

# Unit Tests
test-01 - Configuration Module
test-02 - Data Module
test-03 - Extension Module

# Hardware
## GPS
SAM-M8Q
 - Uses U-BLOX M8 Protocol Version 18

 Data:
  - lat
  - lon
  - alt

  - quality
  - numSats


VTG:
 - Course Over Ground (degrees)
 - Speed over ground km/hr


31.3.2 POSITION (PUBX,00)
 - Vertical Speed - M/s

# Doxygen

To generate Doxygen documentation in PDF form you need to install the following
packages:

-doxygen
-texlive-latex-base
-texlive-latex-recommended 
-texlive-fonts-recommended
-texlive-latex-extra

Style Guide:
    All Doxygen documentation will be done with the Javadoc style.
    Add Classes to appropriate subgroup.
    Each file will have the same format at the top
    Documentation of member functions will go in header files.
    All public and private methods, along with data members need to have
    Doxygen style documentation.

Run from repo root:
doxygen ./docs/Doxygen/Doxyfile

# Old Readme


## This project is not complete and is currently not safe to fly

Here is a very brief and incomplete overview of the system.

### Software  
#### HAACS (High Altitude Autonomous Control System)
*HAACS* is the actual software that will run on the board. This is a much
more advanced version of 'AOS'. Written entirly in C++ it is being made to be
easier to set up and much faster. There are a few basic concept to this system.


All sensors and other data collection tools are refered to as *"Extensions"*.
- This would include external sensors like the DS18B20 temperature probe,
and internal utilities that get system information like cpu temperature,
available memory, etc.


All major functionality of the system is split into *"Modules"*. The most
important modules include:
- Configuration : Reads and parses the flight configuration data.
- Data : Helps pass data between *Extensions* and other modules.
- Extensions : Creates and controls all *Extensions*.
- Telemetry : Handles Telemetry

All of the modules where lots of waiting takes place have multithreading
implemented so they can work while the rest of the system continues on.
This is used in the Telemetry module where a transmission queue allows
the computer to continue on while transmissions are in progress. 
This also is implemented in the Extensions module where each *Extension*
runs in it's own thread. This is helpful as many sensors require some
time for communication. This is where the data module comes in. The
data module contains an object called a "datastream" which utilizes a mutex
to keep the stream safe even when being accessed by multiple threads.

Pulling all of this together is the "FlightRunner" which has a function 
'start()' that starts up and runs the system. It will use all of the modules
to set up the environment before executing the user defined 'Flight Loop'.
The Flight Loop will continue on indefinetly until shutdown.

The flight runner will use the configuration data to determine what extensions
are enabled, what type of telemetry to use, how often to send data packets, and
a whole lot more.

Here is the current list of features that will be included in the basic version. 
* Denotes at least partially implemented.
- APRS Tracking
- SSTV Images
- AFSK Data Packets
- SSDV Images
- Advanced Error Handling
- A failsafe flight procedure
- Automated radio control for switching frequencies automatically
- Two way communication that allows the ground station to send command packets
- A whole lot more that will eventually be documented.

### Hardware
#### BOB (Balloon Ovservation Board)
*BOB* is a open source flight computer based on the Raspberry Pi.
#### AOS (Atmospheric Observation System)
![alt text](https://user-images.githubusercontent.com/19292194/174874402-ad9695dc-447e-425d-afe4-dd6350de5644.jpg)
*AOS* here refers to the old flight board that was not completed but was very
close to being completed.



#### Custom Hardware is not needed

The Flight Computer software has been made to support many different single
board computers. This software is not just specific to the **BOB** It is also made to be easily expanded. Adding sensors is
as simple as adding another "extension" C++ file and then adding it to
your configuration. This project is constantly changing so the documentation
is poor.

The project is currently built with cmake.
Documentation is generated by Doxygen.

All CPP code within HAAC is my own unless otherwise noted in the doxygen
comment at the top of all CPP files or here.
    - Sections of the CMakeFiles file are from the official GTest documentation.
    - Sections of the I2C are from the linux manpage.

# Theme
## Colors
- Off White : #F0ECE1
- Flax : #EDCF8F
- Peru : #C97F4E
- Coffee : #6F4A38
- Chestnut : #977359
## Fonts
- iceberg