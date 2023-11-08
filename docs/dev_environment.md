## Linux Packages
libi2c-dev
nlohmann-json3-dev
libgtest-dev
- build with `cd /usr/src/gtest && sudo cmake CMakeLists.txt && sudo make && sudo cp lib/*.a /usr/lib && sudo ln -s /usr/lib/libgtest.a /usr/local/lib/libgtest.a && sudo ln -s /usr/lib/libgtest_main.a /usr/local/lib/libgtest_main.a`


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
  - https://pypi.org/project/json5/
  - Install with `pip3 install json5`

## Static Analysis/Pre-Commit
- `pip install pre-commit` - see https://pre-commit.com/hooks.html
- `pip install cmake-lint`
- `pip install cmake-format`