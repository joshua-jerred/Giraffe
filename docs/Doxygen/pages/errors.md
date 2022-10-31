## Prefixes:
- ``C_``  Configuration Errors
- ``M_``  Module Errors
- ``E_``  Extension Errors

### C_
C_FNF: File Not Found

#### General:
C_GEN_NF  : General Section not found in file

C_GEN_PN_R [name]: Project Name Out of Range

C_GEN_PN_I [name]: Project name invalid, does not match "^[a-zA-Z_ 0-9-]*$" (Only a-z, 
A-Z, 0-9, _, - and spaces are allowed)

C_GEN_MB_I: Invalid mainboard type, reverting to 'error'.

C_GEN_SP_I: Starting Proc invalid, reverting to 'error'.


#### Extensions

C_EXT_ID_R [id]: ID out of range

C_EXT_ID_S [id]: Extension ID is not sequential.

C_EXT_NM_R [name]: Extension name out of range

C_EXT_NM_I [name]: Extension name invalid. Same requirments as project name but spaces
are not allowed.