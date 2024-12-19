#!/bin/bash
set -e

# A simple script to build and install the Giraffe project

# Must be a valid CMake configuration and build preset
CMAKE_PRESET="host"

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
  log "Building project..."
  # mkdir -p build
  cmake --preset "$CMAKE_PRESET"
  echo "WARNING -- BUILDING WITH HOST CONFIGURATION"
  cmake --build --target giraffe --preset "$CMAKE_PRESET"

  cd build
  log "Build directory: $(pwd)"
  cd ..
}

# -- Install the project
function install_project {
  echo "Installing gcli..."
  sudo cp ./build/$CMAKE_PRESET/bin/gcli /usr/bin
  gcli start
  gcli configure "is_ground_station" "true"
  gcli stop
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
