# Commands

This describes the commands that can be sent to GFS via the server, telemetry,
or console.

All commands are sent as a string of characters/numbers with a known format.

For safety reasons, argument requirements must be met exactly. This means if a
command expects no arguments, there should be nothing after the command ID and
the final `/`.

## Command Format

Commands are separated into 4 sections with a `/` used as a separator.

[Command Flag]/[Category]/[Command ID]/[Command Argument]

The first 3 sections are 3 characters long, with the argument section being
between 0 and 20 characters long.

If there are no arguments for a command, the `/` after the command ID is still
required.

Command Regex:
`` ^cmd\/[a-z]{3}\/[a-z]{3}\/[a-z0-9-]{0,20}$ ``

### Command Flag

Simple just the characters `cmd` to indicate that this is a command.

### Destination

The 'destination' of the command. The target of the command.
Categories are:
    - flr - Flight Runner
    - etm - Extension Module
    - tlm - Telemetry Module
    - csl - Console Module
    - dat - Data Module
    - srv - Server Module
    - sys - System Module
    - ext - Extensions

### Command ID

Also 3 characters long, this is the command ID. The command ID is unique
to the category.

### Command Argument

See the specific command for information about arguments.

With hex arguments, letters must be lowercase.

***

## Command List

### Flight Runner Commands

- `cmd/flr/sdn/` - Shutdown the flight software
- `cmd/flr/mst/[etm|tlm|csl|dat|srv|sys]` - Start the given module
- `cmd/flr/msp/[etm|tlm|csl|dat|srv|sys]` - Stop the given module
- `cmd/flr/mrt/[etm|tlm|csl|dat|srv|sys]` - Restart the given module

## Configuration Commands

- ``cmd/cfg/rlc/`` - Reload the configuration file

### Telemetry Configuration Commands

- ``cmd/cfg/tel/enable=[0|1]`` - Enable or disable telemetry
- ``cmd/cfg/tel/datapacket=[0|1]`` - Enable or disable data packets
  - ``cmd/cfg/tel/dpfreq=[xxx.xxxx]`` - Set the data packet frequency
- ``cmd/cfg/tel/aprs=[0|1]`` - Enable or disable APRS
- ``cmd/cfg/tel/sstv=[0|1]`` - Enable or disable SSTV
  - ``cmd/cfg/tel/sstvmode=[r8|r16|r32]`` - Set the SSTV mode

### Data Module Commands

- ``cmd/dat/cae/`` - Clear all errors from the error frame, they will remain in
  the log
- ``cmd/dat/cse/[xxxx]`` - Clear a specific error given the error code.
  (16-bit hex, leading 0s required)
- ``cmd/dat/ufl/`` - Update the file list
- ``cmd/dat/rlf/`` - Rotate the log file
- ``cmd/dat/ddf/[file name without extension]`` - Delete the data log file with
  the given name
- ``cmd/dat/def/[file name without extension]`` - Delete the error log file with
  the given name

### Extension Commands

- ``cmd/ext/stp/[num]`` - Stop the extension with the given number

### Telemetry Commands

- ``cmd/tlm/rtx/[num]`` - Retransmit the transmission with a given number, as
  long as it is in the log.
- ``cmd/tlm/rsi/`` - Respond with the RSSI of the last received packet
- ``cmd/tlm/snr/`` - Respond with the SNR of the last received packet
- ``cmd/tlm/apl/`` - Send an APRS packet with the current position
