# Configuration Schema

## Process for adding a setting:
1. Document in `src/common/gfs_configuration_metadata.json`
2. Add to `src/common/tests/test-01` and run the test to verify metadata
3. Add validator if needed in `validator.cpp` and `configuration_internal.h`
4. Test the validator in GFS unit test test-01
5. Add to `configuration.h`, add default value to the configuration to the
    configuration class.
6. Add to unit test test-02

## "Rules" To Follow:
- Everything (where possible) must have a default
- No Deep Nesting

`section[setting][setting_field]`

## Required Fields :
- `name`
  - A string containing a human readable name
  - 4 to 25 Characters
- `description`
  - A string containing a description of the setting
  - 20 to 300 Characters
- `type`
    - `bool`
      - true/false
      - Required Additional Fields:
        - `true`: The relevant word for when it's set to true. Ex: "enabled"
        - `false`: Same as above, but for false
    - `string`
      - Required Additional Fields
        - `min`/`max` (Number of characters)
          and/or
        - `pattern` (Regex Pattern to Match)
    - `int`
      - Required Additional Fields
        - `min` Minimum value
        - `max` Maximum Value
        - `step` An integer step value from 1 to n. Ex: A step value of 5 would allow 0, 5, 10, ...
    - `float`
      - Same as with the `int` type, but the step value can be a decimal value
    - `enum`
      - Required Additional Fields
        - `options`
          - An array of strings representing the enum options
    - `array`
      - TBD
    - `struct`
      - TBD
- `default`
  - The default value