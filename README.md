[![giraffe-flight-software tests](https://github.com/joshua-jerred/Giraffe/actions/workflows/gfs-tests.yml/badge.svg)](https://github.com/joshua-jerred/Giraffe/actions/workflows/gfs-tests.yml)
# Giraffe
### A Unified, Semi-Autonomous, High Altitude Balloon Flight Control Framework.
#### Iteration 0.1

<img src="https://user-images.githubusercontent.com/19292194/193437157-0a421f68-fa85-43bb-b372-1807a19c470d.png" height="200">

An extended readme and in depth documentation can be seen here: [http://giraffe.joshuajer.red/](http://giraffe.joshuajer.red/)

Check out the [project](https://github.com/users/joshua-jerred/projects/3) and 
[development branch](https://github.com/joshua-jerred/Giraffe/tree/development) 
for the latest additions.

Currently working on hardware prototypes.

<p float="left">
<img src="https://user-images.githubusercontent.com/19292194/206889714-6b0a2d1d-24f3-48b0-90d8-551f2f527981.png" height="250">
<img src="https://user-images.githubusercontent.com/19292194/206889718-702cd933-8cd7-4ca7-aa9a-46176b9ce56a.png" height="250">
<img src="https://user-images.githubusercontent.com/19292194/206889721-daaeecfa-2522-4d31-946d-b3c4137bdc87.png" height="250">
<img src="https://user-images.githubusercontent.com/19292194/206889719-72e4b6a4-320c-47e9-80a2-8c51772d9031.png" height="250">
</p>

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

Too see my more complete, python implementation of this project refer to [the AOS repo.](https://github.com/joshua-jerred/AOS)

The old version lacked a lot in software, but even more in hardware. There was zero handling of EMI/RFI, it was *bad* but almost functional. The main issue was with the DRA818 module which will be handled a lot better in the future.

<img src="https://user-images.githubusercontent.com/19292194/174874402-ad9695dc-447e-425d-afe4-dd6350de5644.jpg" height="250">
