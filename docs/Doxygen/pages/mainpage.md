# Giraffe
###### A Unified, Semi-Autonomous, High Altitude Balloon Flight Control Framework.

![Logo](logo-small.png)

V0.0.9

Giraffe software is seperated into two sections, the Flight Computer and Ground Station.


# Giraffe Flight Software
## General

GFS, or Giraffe Flight System is what actually runs on the flight computer.

The flight system software consists of modules and utilities which will be 
explained below.

Pulling all of these items together is the [FlightRunner](deadlink). The
FlightRunner is responsible for using all of the modules to run the 
[Flight Loop](deadlink). The flight loops, which are
[configured by the user](deadlink) determine what actions the FlightRunner will
take. The flight loop is the ``what to do, and when to do it`` of the 
FlightRunner.

The basic actions of the FlightRunner are similar to the following:
1. Read the configuration file, check last shutdown state, are you in the middle 
of the flight?
2. Set up the DataModule
3. Set up all Extensions
4. Set up everything else that the user configured
5. Decide which flight loop to start based off of the last shutdown state,
and sensor data.
6. Continue the flight loop until interupted.

## Modules
Modules are the distinct sections of the Flight Computer. Each module
has a specific task. For example, a config module which is responsible for
reading and parsing the configuration file. The telemetry module is
responsible for handling all telemetry.

You can read more about each module on their respective pages.
- ConfigModule
    - Reads and parses configuration json file.
- DataModule
    - The data module is responsible for passing information between modules.
- TelemetryModule
    - The telemetry module handles all telemetry including SSTV, AFSK, APRS, etc.
- ServerModule
    - The server module is what runs the web server which is used for debugging before launch.
- ExtensionsModule
    - The extensions module handles all extensions (sensors).

## Utilities
Utilities are common items that are used between modules. They include
simple libraries and common types.

You can read more about each module on their presective pages.
- [Utility Config Types]()
    - Common configuration data types.
- [Utility Data Stream]()
    - Used by the data module and extensions to pass data between the two
- [Utility Timer]()
    - Super simple timer library usec by the flight runner in the flight loop.






## Other Important Features

### Extensions
"Extensions" refer to each data collection tool or sensor. Each type of 
extension has all of it's own code defined in a single class. Each extension
also inherets a common "extension" class which allows for quick addition of
new extensions and simple integration into the system.

The extensions are started by the flight runner **in their own thread**.
This is important as the extensions have nearly full control over what they do
and when they do it. Each extension interfaces with the data stream which
connects them to the rest of the system.

The extension module is what handles each extension. It creates the objects
and controls the startup, stoping, and restarting of the extensions.

Here is a simple example of how an extension is set up.

1. FlightRunner reads the users extension configuration data with 
the ConfigModule.
2. The FlightRunner launches the Extension Module, passing in
the extension configuration data.
3. The Extension Module will create all of the extension objects
according to configuration data. This includes a name, extension type, update
interval, and a bit more.
Once they are all created it will call the start method, which all extensions
have. This spawns the extension "runner" thread, the thread will continue
until the extension module calls 'stop' on each extension.

A simple extension thread will do the following. The thread will collect data 
from it's sensor, add that data to the data stream, wait a user specified
amount of time, and do it again. The ammount of time is specified in the 
onfiguration of each extension under the value 'update interval'.

Running in this way is helpful because some sensors are very slow and
can take previous time away from other resources.

### Telemetry
Telemetry is all taken care of by the telemetry module. The FlightRunner
will decide when to send data. It can call aprsPacket(location), 
dataPacket(data), sstvImage(image) whenever it wants to. This does not cause
it to transmit right away, instead it will just add a transmission request to
the telemetry transmission queue. An independent thread will look at the queue
and transmit things one at a time. SSTV images can take a couple of minutes,
so APRS and AFSK packets can stack up if needed. They will be sent after the
SSTV transmission is comeplete. This allows the FlightRunner to continue on with
it's work while the telemetry module worries about actually sending everyting.

### Debugging Webserver
Not yet implemented.



# Giraffe Ground Station
Not yet implemented.