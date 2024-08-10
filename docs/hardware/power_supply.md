# Giraffe Flight Computer Power Supply

The power supply for GFC is responsible for taking the input voltage from the
battery and creating a stable 5V and 3.3V output for the whole system.

## Requirements

### Power Output and Consumption

The raspberry pi zero 2 W requires 5V to operate and has a maximum tolerance of
5%. The Raspberry Pi Zero 2 W idles at 280ma and under load can consume up to
600ma. Due to the computational requirements of the flight software, a duty
cycle of 95% should be assumed, so 570.

The radio module, SA868 can operate from 3.3V to 5.5V. The radio module requires
1000ma to transmit at high power when at 5V, 900 at 4.2V. At low transmit power,
the module consumes 550ma at 5V and 450ma at 4.2V. In receive mode, the module
consumes 70ma. The radio module should be assumed to be in receive mode when not
transmitting. Assuming a possible configuration of APRS location and
telemetry packets transmitted (3s, 1 per minute), two way telemetry packets
(3s, 1 per minute), Robot36 SSTV images (36s, every 10 minutes), a realistic
upper bound duty cycle is 15% over an hour, so ~210ma.

All other ICs on board consume a maximum of ~250ma.

High Power TX Peak: 600 + 1000 + 250 = 1850ma
Low Power TX Peak: 600 + 550 + 250 = 1400ma
Duty Cycle Applied: 570 + 210 + 250 = 1030ma

- REQ: The output voltage of the power supply is 5V.
- REQ: The maximum tolerance of the output voltage is 5%, ideally 3% should be targeted.
- REQ: The power supply must be able to provide 2.0A of current.

## Battery Cells

The battery cells should be Lithium-Ion cells. Of the form factors available,
cylindrical cells provide safety benefits over pouch cells. There should be
consideration of the battery being in vacuum and impacting the ground. Looking
at the available form factors, 18650 and 21700 cells are the most widely
available. The 21700 cells have a higher energy density. Given energy density,
decent safety, and availability, NMC/INR cells should be used. The cells should
be from a reputable manufacturer as there are many poor quality products out
there. Capacity should be the priority over current rating as this is a fairly
low power system.

https://archive.org/details/httpsntrs.nasa.govapicitations20100037250downloads20100037250.pdf
https://www.batterypowertips.com/18650-21700-30700-4680-and-other-li-ions-whats-the-difference-faq/
https://blog.satsearch.co/2021-06-23-satellite-batteries-for-cubesats-nanosats-and-other-form-factors
https://web.archive.org/web/20240405003314/https://www.thunderheartreviews.com/2021/12/samsung-21700-50G-48G-50E-test-comparison.html
https://batteryuniversity.com/article/bu-302-series-and-parallel-battery-configurations

- REQ: The battery cells must be 21700 NMC/INR cylindrical Lithium-Ion cells.
- REQ: The battery cells must have a capacity of at least 4000mAh.
- REQ: The battery cells must be from a reputable manufacturer and distributor.


### Cell Configuration and Power Supply Architecture

Depending on the battery cell configuration, the power supply architecture will
change. The battery configurations that can be considered are 1S2P, 2S1P, and
2S2P. The 1S2P configuration would require a boost converter and a single ADC
channel to monitor the battery voltage. The 2S1P/2S2P configurations would
require a buck converter and two ADC channels, or a BMS, to monitor the battery.

- REQ: The battery pack must weigh less than 350g


### Pack Design

### Battery Management

- REQ: The battery SOC should be considered 100% at 4.2V and 0% at 3.0V


INR21700-50E - 17Wh @ 2A
