# ADC Configuration

ADCs are just extensions, so you can configure them like any other
extensions. However, there is a bit of extra configuration that you can
do to make the data more useful! The only field is 'extra_args', which is
simply just a string. By default, just raw ADC values are sent to the
data stream.

## Extra Args Format
Extra arguments are formatted as a comma-separated key-value pair list.
Here is an example:

```
extra_args: "type=v_div,r1=1000,r2=1000,ref=3300"
```

This is what it would look like to configure the ADC as a voltage
divider with a reference voltage of 3.3V and two 1k resistors.

## Valid Configurations
Each ADC type know's it's resolution, so you don't need to specify it.
Any missing arguments will be filled in with default values if they are optional.
A error/warning will be generated if there is a parsing error.

The configuration is rather picky, so make sure you follow the format.

To be safe, all invalid options revert the type to raw count.

### Raw
Arguments:
- type: raw (this is the default, an empty config string will not give an error/warning)

Output:
- raw adc count

Example:
`type=raw`

### Voltage Divider
Arguments:
- type: v_div
- r1: resistance of the first resistor in ohms (1-1000000)
- r2: resistance of the second resistor in ohms (1-1000000)
- ref: reference voltage in millivolts (1-50000) - optional
  - default: 3300

Output:
- voltage in millivolts

Example:
`type=v_div,r1=21312,r2=1,ref=5000`

### Percentage
Arguments:
- type: percent
- min: minimum raw value (0-2^resolution) - optional
  - default: 0
- max: maximum raw value (0-2^16) - optional
  - defaults to the max int of the resolution
- clamp: whether or not to clamp the output to 0-100 - optional
  - default: false

Output:
- number between 0 and 100 (may exceed 0 and 100, output can be negative)

Example:
`type=percent,min=0,max=4095`
`type=percent`