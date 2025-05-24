#!/bin/bash

set -e

CMD_ARG=$1

if [ -z "$CMD_ARG" ]; then
    echo "$0 reboot_power_board"

fi

if [ "$CMD_ARG" == "reboot_power_board" ]; then
    echo "Rebooting power board"
    echo "cmd reboot boot" > $PICO_UART_PATH
fi