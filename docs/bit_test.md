# BIT Test

The GFS BIT (Built-In Test) verifies the functionality of the GFS hardware and
the overall health of the system. The BIT is a series of tests that are run
during the pre-launch phase of the flight. The BIT test is required to pass
before GFS can be transitioned to the launch phase (override available when the
web client is set to 'unsafe' mode).

## Test Sequence

When in the pre-launch phase, the user can run the BIT test via the web client
command center. Immediately after the test is requested, GFS will begin running
the tests.

The upper level test groups are run in parallel, while the lower level tests
within each group are run sequentially.

If a test fails, the user will be presented with the option to retry the test.

When all tests are in either a `PASS` or `SKIP` state, the user can transition
GFS to the launch phase.

## Test Results

As tests are completed, the results are displayed in the web client.

- **NOT RUN** - The test has not been run yet
- **PASS** - The test passed successfully
- **FAIL** - The test failed
- **SKIP** - The test was skipped
- **RUNNING** - The test is currently running

## Test List

### FCS

Test the flight control system.

- ERR  | No reported errors from the flight software
- FPP  | Flight phase prediction is working - high confidence of 'launch' phase
- LPOS | The launch position is set and valid
- DAT  | The GFS data module is running and processing data at an acceptable rate
- DATL | Log files are being created and written to
- DATB | There is a valid database and data is being stored

### D/L

Test the giraffe data link hardware and software.

- The data link is configured to send at least location data.
- The data link has a hardware radio assigned to it.
- The data link is sending and receiving data reliably.

### GPS

Test the GPS hardware and the GPS signal quality.

- GPS hardware exists and is reporting data
- 3D fix is achieved
- A minimum number of satellites are being tracked (6-8)
- GPS signal strength/quality is within acceptable range
- PDOP, HDOP, VDOP are within acceptable range (Dilution of Precision)
- The lock is maintained for a minimum amount of time
- GPS Data is being received at a minimum rate

### BATT

Test the battery monitoring hardware and the battery health.

- Battery voltage/current measuring hardware (ADC) exists and is reporting data
- Battery voltage is within acceptable range
- Battery SOC (State of Charge) is within acceptable range
- Battery balance is within acceptable range

### EXT

Test the extensions hardware.

- All extensions are running and healthy for a minimum amount of time

### ENV

Test for reliable environmental data.

- Temperature sensors are reporting valid data at a minimum rate
- Pressure sensors are reporting valid data at a minimum rate
- Humidity sensors are reporting valid data at a minimum rate

### SYS

- System time sync with the ground station
- CPU/Memory usage is within acceptable range
- System temperature is within acceptable range
- Filesystem is healthy
- Disk space is within acceptable range

### CAM

Test the camera hardware and software if SSTV or image capture is enabled.

### IMU

n/a, future functionality

### ADC

- ADC hardware exists and is reporting data
- ADC data is within acceptable range