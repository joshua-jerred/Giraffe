# Commands
# ========

This describes the commands that can be sent to GFS via the server,
telemetry, or console.

Currently only the server is implemented, but others are soon to follow.

All commands are sent as a string of characters/numbers with a known format.

## Command Format

Commands are separated into 4 sections with a `/` used as a separator.
[Command Flag]/[Category]/[Command ID]/[Command Argument]

The first 3 sections are 3 characters long, with the argument section being
between 0 and 20 characters long.

If there are no arguments for a command, the `/` after the command ID is
still required.

Command Regex:
`` ^cmd\/[a-z]{3}\/[a-z]{3}\/[a-z0-9-]{0,20}$ ``

### Command Flag
Simple just the characters `cmd` to indicate that this is a command.

### Category
The category of the command.
Categories are:
    - flr - Flight Runner
    - tlm - Telemetry
    - ext - Extension
    - mdl - Module

### Command ID
Also 3 characters long, this is the command ID. The command ID is unique
to the category.

### Command Argument
See the specific command

## Command List
### Telemetry
 - ``cmd/tlm/rtx/[num]`` - Retransmit the transmission with a given number, as
   long as it is in the log.