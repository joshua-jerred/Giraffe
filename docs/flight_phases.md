# Flight Phase Identification

## Introduction

Identifying the phase of a balloon flight is important, but can be difficult.

A good example of this is the paper "Phase of flight identification in general aviation operations", by Valentine Pascale Goblet.

Within the paper, some generic parameters are discussed, I'll start my
implementation with these.

## Parameters

- GPS_DFL | D : Distance from Launch (Meters)
  - Lat/Long of the launch location saved, providing a distance.
- GPS_DFG | A : GPS Distance From Ground/Launch Alt (Meters)
- GPS_ALT | a : GPS Altitude (Meters)
- GPS_HS10 | H : Horizontal-Speed-10s (Average over 10 seconds)
- GPS_VS5 | V : Vertical-Speed-5s (Average over 5 seconds)
- GPS_VS60 | v : Vertical-Speed-60s (Average over 60 seconds)

- BAR_DFG | B : Barometer Distance From Ground/Launch Alt (Meters)
- BAR_ALT | b : Barometer Altitude (Meters)
- BAR_VS60 : Barometer Vertical-Speed-60s (Average over 60 seconds)

### Barometer Indicated (Backup)

## Phases

Pre-Launch is ignored, it's a manual transition from pre-launch to launch.

GPS Weights add up to 100

### Launch

| Parameter | Formula/Condition | Weight | Notes                             |
| --------- | ----------------- | ------ | --------------------------------- |
| GPS_DFL   | < 10m + error     | 40     |                                   |
| GPS_DFG   | < 5m + error      | 40     |                                   |
| GPS_HS10  | < 1m/s + error    | 5      |                                   |
| GPS_VS5   | < 0.5m/s + error  | 10     | Added Weight For Faster Detection |
| GPS_VS60  | < 0.5m/s + error  | 5      |                                   |

### Ascent

| Parameter | Formula/Condition | Weight | Notes |
| --------- | ----------------- | ------ | ----- |
| GPS_DFL   | > 10m + error     | 20     |       |
| GPS_DFG   | > 5m + error      | 10     |       |
| GPS_HS10  | > 0.5m/s + error  | 5      |       |
| GPS_VS5   | > 0.5m/s + error  | 10     |       |
| GPS_VS60  | > 0.5m/s + error  | 50     |       |

### Descent

Deviation is added to the GPS_DFG parameter to account for the different
between launch and landing altitudes.

| Parameter | Formula/Condition           | Weight | Notes |
| --------- | --------------------------- | ------ | ----- |
| GPS_DFL   | > 10m + error               | 5      |       |
| GPS_DFG   | > 1500m + error + deviation | 15     |       |
| GPS_HS10  | > 0.5m/s + error            | 10     |       |
| GPS_VS5   | < -1.0m/s + error           | 30     |       |
| GPS_VS60  | < -1.0m/s + error           | 40     |       |

### Recovery

| Parameter | Formula/Condition    | Weight | Notes          |
| --------- | -------------------- | ------ | -------------- |
| GPS_DFL   | < 10m + error        | 10     |                |
| GPS_DFG   | < 5m + error         | 10     |                |
| GPS_HS10  | < 1m/s + error       | 20     | Water Landing? |
| GPS_VS5   | abs < 0.2m/s + error | 30     |                |
| GPS_VS60  | abs < 0.2m/s + error | 40     |                |
