# Build Guide (GFS)

## Build Requirements
#### The following packages must be installed on the host system before building:
- ``cmake``
- ``build-essential``
- ``libi2c-dev``

All can be installed via apt install

#### Automatically downloaded and built via cmake:
- ``nlohmann-json``
- ``googletest (for unit tests)``

## Build Instructions
### Main Branch
``` bash
git clone https://github.com/joshua-jerred/Giraffe.git
cd Giraffe
git submodule update --init --recursive
mkdir build && cd build
cmake ..
sudo make install giraffe-flight-software
```
### Development Branch
``` bash
git clone https://github.com/joshua-jerred/Giraffe.git
cd Giraffe
git switch development
git submodule update --init --recursive
cd Giraffe && mkdir build && cd build
cmake ..
sudo make giraffe-flight-software
```

### Unit Tests
These were made to be run with github actions, but you can run them locally if you want.

See ``./tests/test-list.md`` for a list of tests/test numbers.

From the build directory after running cmake:
``` bash
make test-01 && ../bin/tests/test-01/test-01
```



# Raspberry Pi Configuration
Within raspi-config:
- Enable I2C
- Enable 1-Wire (This requires a reboot!)