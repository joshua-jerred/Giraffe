# Versioning

Based on [semantic version 2.0.0](https://semver.org/).

## How to Update the Version

1. In the file `$REPO/version`, edit the fields under the section `[version]`
   section.

- `major`, `minor`, `patch` = `<uint>`
- `stage` = `alpha`|`beta`|`rc`|`release`
  - If the local repository is dirty, **_this field is ignored_**, and the stage
    is set to `dev`.
  - If set to `release`, the stage identifier is removed from the version
    string.

2. Configure the project with cmake.

Upon configuration, cmake runs `generate_version_files.py`. This script reads
the version values from `$REPO/version`, and then generates version files.

---

@todo

- **_Some files must be updated manually_**:
  - `project/Doxyfile`
  - `README.md`

---

## Versioning Scheme

- `<major>`: Major version number.
  - Incremented when incompatible changes are made, when `0` ignore this rule.
- `<minor>`: Minor version number.
  - Incremented when new features are added in a backwards-compatible manner.
- `<patch>`: Patch version number.
  - Incremented when backwards-compatible bug fixes are made.
- `<stage>`: Release stage identifier.

  - The stage identifier shows the development stage of the version.
  - **_this field is blank for final release versions_**

  | Identifier     | Description                           |
  | -------------- | ------------------------------------- |
  | `dev`          | development (built with dirty branch) |
  | `alpha`        | alpha                                 |
  | `beta`         | beta                                  |
  | `rc`           | release candidate                     |
  | **\- null \-** | release                               |

## Version String Format

Used for package files.

`<major>.<minor>.<patch>[-stage]`

- `<major>`, `<minor>`, `<patch>`:
  - no leading zeros, except when value is `0`
- `[-stage]`:
  - _empty for final release versions_
  - `-` followed by the stage identifier from the table above
