# GFS Errors

## Prefixes:
- ``CG_``  Configuration Errors
- ``M_``  Module Errors
- ``E_``  Extension Errors

## C_ Configuration Errors
FNF: File Not Found

#### General Config
GEN_NF  : General Section not found in file

GEN_PN_R [name]: Project Name Out of Range

GEN_PN_I [name]: Project name invalid, does not match "^[a-zA-Z_ 0-9-]*$" (Only a-z, 
A-Z, 0-9, _, - and spaces are allowed)

GEN_MB_I: Invalid mainboard type, reverting to 'error'.

GEN_SP_I: Starting Proc invalid, reverting to 'error'.


#### Extension Config

EXT_ID_R [id]: ID out of range

EXT_ID_S [id]: Extension ID is not sequential.

EXT_NM_R [name]: Extension name out of range

EXT_NM_I [name]: Extension name invalid. Same requirements as project name but spaces are not allowed.

EXT_OW_I [id]: One Wire ID does not match pattern

EXT_I2_B [buss-num]: I2C bus number out of range

EXT_I2_A [address]: I2C address out of range

EXT_MISS [missing-item : extension-name]: Missing item in extension configuration

#### Telemetry Config
TL_PSK_F [_]: psk-mode or speed not found, defaulting to BPSK_125
TL_PSK_M [mode]: Invalid PSK31 mode (not BPSK or QPSK)
TL_PSK_S [speed]: Invalid PSK31 speed (not 125, 250, or 500)



## Telemetry Module Errors
#### PSK31
PSK_M: Invalid PSK31 mode (not BPSK or QPSK) - Reverting to BPSK
PSK_S: Invalid PSK31 Symbol Rate (not 125, 250, or 500) - Reverting to 125
PSK_E: Failed to encode message