#!/bin/bash
set -e

# A simple script to build and install the Giraffe project

# gcli install directory
INSTALL_DIR="/usr/bin"
GCLI_DATA_DIR="~/.giraffe/agent"

# Must be a valid CMake configuration and build preset
CMAKE_PRESET="host"
BUILD_TYPE="Debug"

BASE_DIR="$(pwd)"
SRC_DIR="$BASE_DIR/src"
CMAKE_BUILD_DIR="$BASE_DIR/build/$CMAKE_PRESET"
CMAKE_BIN_DIR="$CMAKE_BUILD_DIR/bin"

NON_INTERACTIVE_OPT="y"
NON_INT_SILENT_OPT="-s"

if [[ $1 == "help" ]]; then
  # echo "Usage: ./install.sh <options> [$NON_INTERACTIVE_OPT]
  echo "Usage: ./install.sh <options> [$NON_INTERACTIVE_OPT, $NON_INT_SILENT_OPT]
<options>:
  setup[default]: Setup the project. Run deps, build, and install
  deps: Install dependencies
  build: Build the project
  install: Install the project
  help: Display this help message
$NON_INTERACTIVE_OPT: Run in non-interactive mode (yes to all prompts)
$NON_INT_SILENT_OPT: Run in silent non-interactive mode (no stdout and yes to all prompts)"
  exit 0
fi

INTERACTIVE="true"
SILENT="false"
if [ $# -gt 0 ]; then
  if [ "$1" = "$NON_INTERACTIVE_OPT" ] || [[ $# -gt 1 && "$2" = "$NON_INTERACTIVE_OPT" ]]; then
    echo "Running in non-interactive mode"
    INTERACTIVE="false"
  fi
  if [ "$1" = "$NON_INT_SILENT_OPT" ] || [[ $# -gt 1 && "$2" = "$NON_INT_SILENT_OPT" ]]; then
    INTERACTIVE="false"
    SILENT="true"
  fi
fi

# A function which will prompt the user for input
# if no input is provided, the default value will be used.
function prompt {
  local message=$1
  local default=$2
  local answer

  if [ $INTERACTIVE == "true" ]; then
    if [ -z "$default" ]; then
      read -p "$message" answer
    else
      read -p "$message " answer
    fi
    echo ${answer:-$default}
  else
    echo ${default}
  fi
}

function log {
  if [ $SILENT == "false" ]; then
    echo $1
  fi
}

# -- Install dependencies
function install_dependencies {
  echo "Installing dependencies..."
  # @todo update from build script in docs
}

# -- Build the project
function build_project {
  log "==Building project..."
  # mkdir -p build

  log "CMake Configure -- Preset: $CMAKE_PRESET"
  cmake --preset "$CMAKE_PRESET"
  log "CMake Build -- Preset: $CMAKE_PRESET"
  cmake --build --target giraffe --preset "$CMAKE_PRESET"

  cd $SRC_DIR/ground_station
  log "install node modules (@todo normally not needed, but given the nature of the project, an internet connection isn't guaranteed)"
  # @todo add ci arg to the npm install command
  npm install

  log "==Creating Giraffe Software Package"
  log "Package ground station"
  tar -czf $CMAKE_BIN_DIR/ground_station.tar.gz *
  echo $CMAKE_BIN_DIR

  cd $CMAKE_BIN_DIR
  rm -rf giraffe
  mkdir -p giraffe
  cp gcli giraffe
  mv ground_station.tar.gz giraffe

  # cd build/$CMAKE_PRESET
  # log "build directory: $(pwd)"
  # WORK_DIR="$(pwd)/bin"
  # log "output directory: $WORK_DIR"
  # echo $CMAKE_BIN_DIR
}

# -- Install the project
function install_project {
  log "Installing gcli..."

  # Copy and configure must be done while gcli is not running. If it exists,
  # stop it, but ignore any errors.
  gcli stop -allow-keep-state || true

  # Create the gcli data directory if it doesn't exist
  mkdir -p $GCLI_DATA_DIR

  log "Copying gcli to $INSTALL_DIR"
  sudo cp ./build/$CMAKE_PRESET/bin/gcli $INSTALL_DIR

  if [ $(prompt "Configure gcli as ground station (y, default) or flight computer (n)? [y/n|default y]" "y") == "y" ]; then
    log "Configuring gcli as ground station"
    gcli configure "is_ground_station" "true"
  else
    log "Configuring gcli as flight computer"
    gcli configure "is_ground_station" "false"
  fi

  # if [ $BUILD_TYPE == "Debug" ]; then
    # gcli configure dev_mode true
  # fi

  log "Starting gcli as a background process"
  gcli start
}

function setup {
  if [ $(prompt "Install Dependencies? [y/n|default y]" "y") == "y" ]; then
    install_dependencies
  fi
  if [ $(prompt "Build primary Giraffe components? [y/n|default y]" "y") == "y" ]; then
    build_project
  fi
  if [ $(prompt "Install Giraffe CLI? [y/n|default y]" "y") == "y" ]; then
    install_project
  fi
}

# capture "$NON_INTERACTIVE_OPT" as arg 1 here to allow non-interactive mode setup by default
if [ $# == 0 ]; then
  echo "No arguments provided, running default setup. Run './install.sh help' for more options."
fi
if [ $# == 0 ] || [ "$1" = "setup" ] || [ "$1" = "$NON_INTERACTIVE_OPT" ] || [ "$1" = "$NON_INT_SILENT_OPT" ]; then
  setup
  exit 0
fi

if [ $1 == "deps" ]; then
  install_dependencies
  exit 0
fi

if [ $1 == "build" ]; then
  build_project
  exit 0
fi

if [ $1 == "install" ]; then
  install_project
  exit 0
fi

echo "Invalid argument provided. Run './install.sh help' for more options."
exit 1
