# Giraffe Application Structure

As everything is under 1.0, this is very much a working document.

## Giraffe Package

Giraffe relies on 5+ different applications across multiple systems. To keep
things "simple", everything is bundled into a single package. Ultimately, the
target for this package is debian-based systems, so dpkg is a valid option.

A custom rolled package _was_ attempted, but that is a layer of complexity that
this project _does not need_ before the first flight, it was abandoned in favor
of dpkg.

### dpkg

### DEPRECATED - Package File

**Filetype**: `tarball` compressed with `gzip`, so `tar -xzf`

- @todo consider a change to bzip2 as there is already a library for it in the
  project. This would require a tar library as well. Worth considering for the
  future.

**Filename**: `giraffe-<full_version_string>.tar.gz`

- `<full_version_string>` The full version string as described in
  `docs/versioning.md`.

**Example**: `giraffe_1.0.0.tar.gz` - The first stable release of Giraffe.
**Example**: `giraffe_1.0.0-alpha-dev.tar.gz` - A giraffe alpha version, this
code has not been committed yet.

### Package Contents

The package contents that are defined here are implemented in `package.cmake`.

- `giraffe-<full_version_string>.tar.gz`
  - `giraffe-<full_version_string>/`
    - `software_manifest.json` - JSON file containing the update manifest
    - `giraffe_daemon` - daemon binary
    - `gcli` - command line binary, aliased to `giraffe`
    - `gfs` - flight software
    - `gdl_server` - data link server
    - `ground_station.tar.gz` - packaged nodejs application
      - `giraffe_ground_station/`
        - `package.json`
        - And the rest of the nodejs application
