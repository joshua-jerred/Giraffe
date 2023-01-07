[![giraffe-flight-software tests](https://github.com/joshua-jerred/Giraffe/actions/workflows/gfs-tests.yml/badge.svg)](https://github.com/joshua-jerred/Giraffe/actions/workflows/gfs-tests.yml)
# Giraffe
### A Unified, Semi-Autonomous, High Altitude Balloon Flight Control Framework.
### [Iteration 0.3](https://giraffe.joshuajer.red/md_docs_Doxygen_pages_versions.html)

<img src="https://user-images.githubusercontent.com/19292194/193437157-0a421f68-fa85-43bb-b372-1807a19c470d.png" height="200">

An extended readme and in depth documentation can be seen here: [http://giraffe.joshuajer.red/](http://giraffe.joshuajer.red/)

You can read about why/what I'm doing this here: [https://joshuajer.red/Giraffe-Introduction/](https://joshuajer.red/Giraffe-Introduction/)

Check out the [project](https://github.com/users/joshua-jerred/projects/3) and 
[development branch](https://github.com/joshua-jerred/Giraffe/tree/development) 
for the latest additions.

---

Currently working on hardware prototypes.
The radio/telemetry module will be on a separate board.
##### GFC Hat Prototype 1
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

##### GFC Hat Prototype 2 (In progress)
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
<img src="https://user-images.githubusercontent.com/19292194/211131524-67701138-8dad-438a-b97c-4af60ae09b90.png" height="400">
</p>

##### GFC "Shoe" Prototype 1
 - In progress, currently working with modular prototypes. This will go below the Pi Zero.
 - Embedded USB sound card and DRA818V module with LPF.
---
# About
Giraffe is a flight control framework for high altitude balloon (HAB) flights. 
The framework includes flight computer hardware and software, as well as 
ground station software. 

Everything is designed to be modular and extensible so that it can be used for
all types of HAB flights. The software is made to work on any ARM based single board
computer that runs Linux, like the Raspberry Pi. The framework utilizes open 
communication protocols like APRS, AFSK, and SSTV to allow for easy integration
into existing software. Two way communication allows you to send commands to the
flight computer from the ground station. There is a lot more included that isn't
mentioned here, so please see the [rest of the documentation](http://giraffe.joshuajer.red/).

Too see my more simplified, but more complete, python implementation of this project refer to [the AOS repo.](https://github.com/joshua-jerred/AOS)

# Interface Examples
![image](https://user-images.githubusercontent.com/19292194/211131871-5038b9aa-9065-464f-87f6-c8db80974435.png)
![image](https://user-images.githubusercontent.com/19292194/211131885-df111ace-09ca-445c-bf5d-a0eb66be1097.png)
![image](https://user-images.githubusercontent.com/19292194/211131919-e34518e6-2f79-4cfe-aa36-dedf77819e29.png)



The old version lacked a lot in software, but even more in hardware. There was zero handling of EMI/RFI, it was *bad* but almost functional. The main issue was with the DRA818 module which will be handled a lot better in the future.
<img src="https://user-images.githubusercontent.com/19292194/174874402-ad9695dc-447e-425d-afe4-dd6350de5644.jpg" height="250">
