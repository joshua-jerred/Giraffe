[![giraffe-flight-software tests](https://github.com/joshua-jerred/Giraffe/actions/workflows/gfs-tests.yml/badge.svg)](https://github.com/joshua-jerred/Giraffe/actions/workflows/gfs-tests.yml)
# Giraffe
### A Unified, Semi-Autonomous, High Altitude Balloon Flight Control Framework.
#### Iteration 0.1

An extended readme and in depth documentation can be seen here: [http://giraffe.joshuajer.red/](http://giraffe.joshuajer.red/)

Check out the [project](https://github.com/users/joshua-jerred/projects/3) and 
[development branch](https://github.com/joshua-jerred/Giraffe/tree/development) 
for the latest additions.

![logo](https://user-images.githubusercontent.com/19292194/193437157-0a421f68-fa85-43bb-b372-1807a19c470d.png)


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

![alt text](https://user-images.githubusercontent.com/19292194/174874402-ad9695dc-447e-425d-afe4-dd6350de5644.jpg)
