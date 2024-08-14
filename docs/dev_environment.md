# Development Environment

## Linux Packages

libi2c-dev
nlohmann-json3-dev
libgtest-dev
libncurses5-dev
libncursesw5-dev
libpulse-dev

libgtest-dev needed to be manually installed on the docker cross compiler

- `cd /usr/src/gtest`
- `sudo cmake CMakeLists.txt`
- `sudo make`
- `sudo cp lib/*.a /usr/lib`
- `sudo ln -s /usr/lib/libgtest.a /usr/local/lib/libgtest.a`
- `sudo ln -s /usr/lib/libgtest_main.a /usr/local/lib/libgtest_main.a`

## Cross Compiler GMock

Normally libgtest-dev does everything, but for the docker cross compiler I had
to do the following:

- `cd /usr/src/gmock`
- `sudo cmake CMakeLists.txt && sudo make`
- `sudo cp lib/*.a /usr/lib/arm-linux-gnueabihf/`

Without this cmake was not able to find gmock.

## Python

Python 3 is required, created and tested with Python 3.10.6
The following pip packages are required:

- `json5` (pyjson5)
  - For running C++ generator scripts
  - <https://pypi.org/project/json5/>
  - Install with `pip3 install json5`

## Static Analysis/Pre-Commit

## Pre-Commit

Need the following packages:

- `codespell`
- `pre-commit`
- `cmake-lint`
- `cmake-format`
- `cpptest`

- Install on your local machine with `pip install pre-commit`
- Run `pre-commit install` in the root of the repo
- Run `pre-commit run --all-files` to run all checks
