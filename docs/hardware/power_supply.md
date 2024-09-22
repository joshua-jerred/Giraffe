# Giraffe Flight Computer Power Supply

The power supply for GFC is responsible for taking the input voltage from the
battery and creating a stable 5V and 3.3V output for the whole system.

***

## Requirements

### Power Output and Consumption

The raspberry pi zero 2 W requires 5V to operate and has a maximum tolerance of
5%. The Raspberry Pi Zero 2 W idles at 280ma and under load can consume up to
600ma. Due to the computational requirements of the flight software, a duty
cycle of 95% should be assumed.

The radio module, SA868 can operate from 3.3V to 5.5V. The radio module requires
1000ma to transmit at high power when at 5V, 900 at 4.2V. At low transmit power,
the module consumes 550ma at 5V and 450ma at 4.2V. In receive mode, the module
consumes 70ma. The radio module should be assumed to be in receive mode when not
transmitting. Assuming a possible configuration of APRS location and
telemetry packets transmitted (3s, 1 per minute), two way telemetry packets
(3s, 1 per minute), Robot36 SSTV images (36s, every 10 minutes), a realistic
upper bound duty cycle is 15%.

The Pi camera module requires 250ma to operate. This needs to be tested, but for
now assume that it takes 3 seconds to power on, take a picture, and power off.
If a picture is taken every 30 seconds, the duty cycle is 10%, so ~25ma.

As things are still changing, there is no point in trying to calculate the power
consumption of the other onboard ICs. After a quick run through, it should be
less 200ma at peak. This needs to be tested.

The flight hardware does have connectors for external sensors. The DS18B20
sensors are negligible. There is a single I2C connector available for expansion.
100ma should be allocated for this. This also means that the Pi's 3.3V rail
can not be used for the 3.3V components.

The following table summarizes the power consumption of the components.
**Note**: The table below does not distinguish between 3.3V and 5V components.
The 3.3V components are are a small fraction of the total power consumption.

| Component                 | Base Current (ma) | Peak Current (ma) | Duty Cycle | Average Current (ma) |
| ------------------------- | ----------------- | ----------------- | ---------- | -------------------- |
| Raspberry Pi Zero 2 W     | 280               | 600               | 95%        | 584                  |
<!-- | SA868 Radio Module (High) | 70                | 1000              | 15%        | 210                  | -->
| Pi Camera Module          | 0                 | 250               | 5%         | 25                   |
| Other Onboard ICs         | 50                | 200               | 5%         | 125                  |
| External Sensors          | 0                 | 100               | 5%         | 5                    |
| **Total**                 | **400**           | **1150**          |            | **949**              |

- REQ: The battery voltage will be regulated to 5V with a buck or boost converter, dependant on pack configuration.
- REQ: The 5V regulator must be able to supply at least 2.5A.
- REQ: The 3.3V bus will be regulated from the 5V bus with an LDO.
- REQ: The acceptable voltage tolerance of 3.3V and 5V bus is 5%.
- REQ: The power supply will have current sensing.

### Battery Cells

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

***

## Design

### Battery

Going with a 2SxP configuration, the power supply will consist of a buck
converter.

Tentatively, the cell choice is INR21700-50E, which has is 17Wh at a 2A draw.

### Current Sensing

High side current sensing will be used with the shunt resistor placed between
the battery and the buck converter.

Currently looking at the INA226 for current sensing.

INA180A

### Buck-Converter

Decided on the TPS56320X, specifically the TPS563201. For $0.43, it meets all
requirements, goes up to 3A, and comes in a easier to use SOT-23 (Thin) package.

The TPS563201 specifically uses Pulse Skip Mode, because it's not CCM it does
not have a fixed frequency. The hope is that this will not be an issue EMI/EMC
wise.