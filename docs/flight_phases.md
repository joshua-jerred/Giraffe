# Flight Phases

## Introduction

The flight computer has the ability to detect the phase of balloon flight based
on a number of parameters. This is used for controlling various aspects of the
system.

## Phases

### Pre-Launch

The initial state of the flight computer, before launch. The flight computer can
only transition to this phase with user input via the web client (TCP GFS
Connection / on the ground only, no telemetry).

When the flight computer is in this phase it will perform a series of checks to
ensure that the system is ready for launch. Once these checks are complete, the
user can enter the launch phase.

#### Transition In

The transition into this phase is manual and done by the user on the web client
when the GFS is connected via TCP/on the ground. It can only be done when the
flight computer is detecting that it is in the launch, recovery, or unknown
phases.

#### Transition Out
The transition into this phase is also done via the web client. The user can
enter the launch phase when the system is ready.

If the flight computer is in flight when in the launch phase, it will
automatically transition to a flight phase.

### Launch
This phase is entered when the user sends a launch command to the flight
computer via the web interface.

### Ascent
### Descent
### Recovery


***

## Flight Phase Detection Model

This is useful for a number of reasons, including:
- Logging data about specific events
- Triggering/Sequencing events
- Changing the behavior of the flight computer
- A number of other things

### Warning!
A whole lot of this information needs to be updated, the model has changed
significantly since this was written.

### Introduction

Identifying the phase of a balloon flight is important, but can be difficult.

A good example of this is the paper "Phase of flight identification in general aviation operations", by Valentine Pascale Goblet.

Within the paper, some generic parameters are discussed, I'll start my
implementation with these.

### Parameters

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

#### Barometer Indicated (Backup)

### Phases

Pre-Launch is ignored, it's a manual transition from pre-launch to launch.

GPS Weights add up to 100

#### Launch

| Parameter | Formula/Condition | Weight | Notes                             |
| --------- | ----------------- | ------ | --------------------------------- |
| GPS_DFL   | < 10m + error     | 40     |                                   |
| GPS_DFG   | < 5m + error      | 40     |                                   |
| GPS_HS10  | < 1m/s + error    | 5      |                                   |
| GPS_VS5   | < 0.5m/s + error  | 10     | Added Weight For Faster Detection |
| GPS_VS60  | < 0.5m/s + error  | 5      |                                   |

#### Ascent

| Parameter | Formula/Condition | Weight | Notes |
| --------- | ----------------- | ------ | ----- |
| GPS_DFL   | > 10m + error     | 20     |       |
| GPS_DFG   | > 5m + error      | 10     |       |
| GPS_HS10  | > 0.5m/s + error  | 5      |       |
| GPS_VS5   | > 0.5m/s + error  | 10     |       |
| GPS_VS60  | > 0.5m/s + error  | 50     |       |

#### Descent

Deviation is added to the GPS_DFG parameter to account for the different
between launch and landing altitudes.

| Parameter | Formula/Condition           | Weight | Notes |
| --------- | --------------------------- | ------ | ----- |
| GPS_DFL   | > 10m + error               | 5      |       |
| GPS_DFG   | > 1500m + error + deviation | 15     |       |
| GPS_HS10  | > 0.5m/s + error            | 10     |       |
| GPS_VS5   | < -1.0m/s + error           | 30     |       |
| GPS_VS60  | < -1.0m/s + error           | 40     |       |

#### Recovery

| Parameter | Formula/Condition    | Weight | Notes          |
| --------- | -------------------- | ------ | -------------- |
| GPS_DFL   | < 10m + error        | 10     |                |
| GPS_DFG   | < 5m + error         | 10     |                |
| GPS_HS10  | < 1m/s + error       | 20     | Water Landing? |
| GPS_VS5   | abs < 0.2m/s + error | 30     |                |
| GPS_VS60  | abs < 0.2m/s + error | 40     |                |
