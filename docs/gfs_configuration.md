# Configuration Schema

The process of adding configuration options has been fully automated.

## Process for adding a setting:
1. Add to `project/metadata/gfs_configuration.json`
2. Generate the configuration files with the VSCode task or by running
   `project/generate_source_files.sh`

## Guidelines to Follow:
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