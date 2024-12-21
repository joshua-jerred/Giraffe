# Giraffe Application Structure

## Giraffe Package

Giraffe relies on 5+ different applications across multiple systems. To keep
things "simple", everything is bundled into a single package.

### Package File

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

- `giraffe-<full_version_string>/`
  - `
