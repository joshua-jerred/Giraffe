# GFS Errors

``[Prefix][Source]_[Error Code] : [Info]`` Info is normally numeric, but can be a string.

## Prefixes:  
- ``M_``  Module Errors
  - ``M_DA`` Data Module
  - ``M_CF`` Configuration Module
  - ``M_CO`` Console Module
  - ``M_EX`` Extension Module
  - ``M_SV`` Server Module
  - ``M_TL`` Telemetry Module
- ``EX_<Extension Numerical ID>``  Extension Errors, ie EX_1, EX_2, etc.




## C_ Configuration Errors
 - FNF: File Not Found

#### General Config
 - GEN_NF  : General Section not found in file
 - GEN_PN_R [name]: Project Name Out of Range
 - GEN_PN_I [name]: Project name invalid, does not match "^[a-zA-Z_ 0-9-]*$" 
  (Only a-z, A-Z, 0-9, _, - and spaces are allowed)
 - GEN_MB_I: Invalid mainboard type, reverting to 'error'.
 - GEN_SP_I: Starting Proc invalid, reverting to 'error'.

#### Extension Config
 - EXT_ID_R [id]: ID out of range
 - EXT_ID_S [id]: Extension ID is not sequential.
 - EXT_NM_R [name]: Extension name out of range
 - EXT_NM_I [name]: Extension name invalid. Same requirements as project name but spaces are not allowed.
 - EXT_OW_I [id]: One Wire ID does not match pattern
 - EXT_I2_B [buss-num]: I2C bus number out of range
 - EXT_I2_A [address]: I2C address out of range
 - EXT_MISS [missing-item : extension-name]: Missing item in extension configuration
 - EXT_UI_R [interval]: Update interval out of range, reverting to 1000ms
#### Telemetry Config
 - TL_PSK_F [_]: psk-mode or speed not found, defaulting to BPSK_125
 - TL_PSK_M [mode]: Invalid PSK31 mode (not BPSK or QPSK)
 - TL_PSK_S [speed]: Invalid PSK31 speed (not 125, 250, or 500)


## Telemetry Module Errors
### PSK31
 - M_TL_PSK_M: Invalid PSK31 mode (not BPSK or QPSK) - Reverting to BPSK
 - M_TL_PSK_S: Invalid PSK31 Symbol Rate (not 125, 250, or 500) - Reverting to 125
 - M_TL_PSK_E: Failed to encode message

## Extension Errors - All start with (EX_<ID>)
### BMP180
 - HSK_F: Failed to handshake with BMP180
 - HSK_U: Unrecognized handshake response from BMP180
 - I2C_CB: Bad configuration for I2C bus (bus num out of range)
 - I2C_CA: Bad configuration for I2C address (address out of range)
 - I2C_BE: Failed to open I2C bus
 - I2C_AE: Failed to configure I2C address
 - I2C_CU: Unknown error connecting to I2C bus
 - CD_R: I2C errors while reading calibration data
 - CD_U: Calibration data is invalid (Not implemented)
 - RT: Error reading raw temperature
 - RP: Error reading raw pressure
 - P_SLP: Error with usleep, should be accompanied by another error
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
