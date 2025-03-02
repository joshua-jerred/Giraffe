#!/bin/bash

set -e

echo "Uploading to Pico: $1"

# Boot into bootloader mode. This will fail if the Pico is already there.
picotool reboot -f -u || true
sleep 1 # Give the Pico time to reboot

# Upload the program
picotool load "$1"
picotool verify "$1"

# Reboot into the application
picotool reboot -a