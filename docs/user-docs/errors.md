# GFS Errors

``[Prefix][Source]_[Error Code] : [Info]`` Info is normally numeric, but can be a string.

## Prefixes:  
- ``M_``  Module Errors
  - ``M_CF`` Configuration Module
  - ``M_DA`` Data Module
  - ``M_CO`` Console Module
  - ``M_EX`` Extension Module
  - ``M_SV`` Server Module
  - ``M_TL`` Telemetry Module
- ``EX_<extension id>``     Extension Errors, ie EX_1, EX_2, etc.
- ``DS_`` DataStream Errors

## Command Errors
- ``<any_prefix>_CMD_NF <command id>``
  - Command Not Found
- ``<any_prefix>_CMD_A <command id>$<command args>$``
  - Command Arguments Invalid

## Generic Module Errors
- ``M_**_MCNI``
  - Module Command Structure Not Implemented
- ``M_**_MCQF``
  - Module Command Queue Full

## Configuration Module
- ``M_CF_FNF``
  - Config File Not Found
- ``M_CF_MGPS``
  - Multiple GPS Data Sources, choosing first
- ``M_CF_MBAT``
  - Multiple Battery Data Sources, choosing first
- ``M_CF_MSYS``
  - Multiple System Data Sources, choosing first
- ``M_CF_MRAD``
  - Multiple Radio Data Sources, choosing first

#### General Config
- ``M_CF_GEN_NF`` 
  - General Section not found in file
- ``M_CF_GEN_PN_R ``
  - Project Name Out of Range
- ``M_CF_GEN_PN_I <name>`` 
  - Project name invalid, does not match "\^[a-zA-Z_ 0-9-]*$" (Only a-z, A-Z, 0-9, _, - and spaces are allowed)
- ``M_CF_GEN_MB_NF``
  - Mainboard not found in config file
- ``M_CF_GEN_MB_I``
  - Invalid mainboard type, reverting to 'error'.
- ``M_CF_GEN_SP_NF``
  - Starting Proc not found in config file
- ``M_CF_GEN_SP_I``
  - Starting Proc invalid, reverting to 'error'.

#### Extension Config
- ``M_CF_EXT_ID_R <id>``
  - ID out of range
- ``M_CF_EXT_ID_S <id>``
  - Extension ID is not sequential.
- ``M_CF_EXT_NM_R <name>``
  - Extension name out of range
- ``M_CF_EXT_NM_I <name>``
  - Extension name invalid. Same requirements as project name but spaces are not allowed.
- ``M_CF_EXT_TP_R``
  - Extensions type out of range
- ``M_CF_EXT_OW_I <id>`` 
  - One Wire ID does not match pattern
- ``M_CF_EXT_I2_B <buss-num>`` 
  - I2C bus number out of range
- ``M_CF_EXT_I2_A <address>`` 
  - I2C address out of range
- ``M_CF_EXT_MISS <address>`` 
  - Missing item in extension configuration
- ``M_CF_EXT_UI_R <interval>`` 
  - Update interval out of range, reverting to 1000ms

#### Telemetry Config
- ``TL_PSK_F <mode>`` 
  - psk-mode or speed not found, defaulting to BPSK_125
- ``TL_PSK_M <mode>`` 
  - Invalid PSK31 mode (not BPSK or QPSK)
- ``TL_PSK_S <speed>`` 
  - Invalid PSK31 speed (not 125, 250, or 500)

## Data Module Errors
- ``M_DA_NGPS``
  - No GPS Data Source
- ``M_DA_NBAT``
  - No Battery Data Source
- ``M_DA_NSYS``
  - No System Data Source
- ``M_DA_NRAD``
  - No Radio Data Source
- ``M_DA_CD <unit>``
  - Critical Data not found for unit
- ``M_DA_CDPE <unit>``
  - Critical Data Parse Error
- ``M_DA_NLOG``
  - Could not open log file when reading size

## Telemetry Module Errors
#### PSK31
 - ``PSK_M`` 
   - Invalid PSK31 mode (not BPSK or QPSK) - Reverting to BPSK
 - ``PSK_S`` 
   - Invalid PSK31 Symbol Rate (not 125, 250, or 500) - Reverting to 125
 - ``PSK_E`` 
   - Failed to encode message
 - ``TX_LOG_SZ``: Internal error, tx log is not the correct size
 - ``DEL_WAV``: Internal error, failed to delete wav file
 - ``APLAY_EXIT_CODE``[errno], [wav_location]: Internal error, aplay
   `` exited`` with a non-zero exit code that was not handled.
 - ``RTX_INV`` Request transmission command has an invalid argument.
 - ``RTX_NF`` 
   - Requested transmission not found in the log.
 - ``RTX_MF`` 
   - Modulation failure while encoding old transmission.

## Extension Module Errors
 - ``M_EX_STS <extension id>``
   - Internal error, extension status unknown

## Extension Errors
### BMP180
 - HSK_F
   - Failed to handshake with BMP180
 - HSK_U
   - Unrecognized handshake response from BMP180
 - I2C_CB
   - Bad configuration for I2C bus (bus num out of range)
 - I2C_CA
   - Bad configuration for I2C address (address out of range)
 - I2C_BE
   - Failed to open I2C bus
 - I2C_AE
   - Failed to configure I2C address
 - I2C_CU
   - Unknown error connecting to I2C bus
 - CD_R
   - I2C errors while reading calibration data
 - CD_U
   - Calibration data is invalid (Not implemented)
 - RT
   - Error reading raw temperature
 - RP
   - Error reading raw pressure
 - P_SLP
   - Error with usleep, should be accompanied by another error
 - DIV0: Prevented division by zero in temperature or pressure calculation, should be accompanied by another error
 - TBAR: Temperature below accuracy range (-40 C)
 - TAAR: Temperature above accuracy range (85 C)
 - PBAR: Pressure below accuracy range (305 hPa)
 - PAAR: Pressure above accuracy range (1100 hPa)

### DS18B20
 - 1WNF: One Wire Device Not Found
 - 1WNR: One Wire Not Readable (Bus was read, but data was non-integer)
 - 1WNDP: One Wire No Data Present
 - GRE: General Read Error

### BME280
 - HSK_F: Failed to handshake with BME280 (I2C error)
 - HSK_U: Unrecognized handshake response from BME280
 - I2C_CB: Bad configuration for I2C bus (bus num out of range)
 - I2C_CA: Bad configuration for I2C address (address out of range)
 - I2C_BE: Failed to open I2C bus
 - I2C_AE: Failed to configure I2C address
 - I2C_CU: Unknown error connecting to I2C bus
 - CFG: Failed to write configuration to BME280
 - CD_R: I2C errors while reading calibration data
 - PRS: Error parsing data
 - RD: Error reading data
 - DIV0: Prevented division by zero in temperature or pressure calculation, should be accompanied by another error
 - TBAR: Temperature below accuracy range (-40 C)
 - TAAR: Temperature above accuracy range (85 C)
 - PBAR: Pressure below accuracy range (305 hPa)
 - PAAR: Pressure above accuracy range (1100 hPa)
 - RHOR: Relative Humidity out of range (0-100%)

### SAM-M8Q
 - ``CFG_T``
   - Configuration failed, reached timeout.

## DataStream Errors
 - CMD_S [command]: Command string too short
 - CMD_L [command]: Command string too long, only provides the first 30 characters
 - CMD_Q [command]: Command queue is full
 - CMD_B [command]: Command format is invalid
 - CMD_C [command]: Unrecognized category