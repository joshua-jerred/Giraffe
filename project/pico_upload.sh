#!/bin/bash

PICO_UART_PATH=/dev/ttyUSB0

set -e

echo "Uploading to Pico: $1"

# Boot into bootloader mode. This will fail if the Pico is already there.
# picotool reboot -f -u || true
# sleep 1 # Give the Pico time to reboot
echo "cmd reboot boot" > $PICO_UART_PATH || true
sleep 1

# Upload the program
picotool load "$1"
picotool verify "$1"

# Reboot into the application, use -f to force the reboot
echo "Booting into application"
picotool reboot -a -f
