#!/bin/bash

set -e
set -u

echo "This script is meant to fully set up the ground station."
echo "It is assued that this is running on a raspberry pi or other linux machine."

# Direwolf SDR
# rtl_fm -f 144390000 -s 22050 | direwolf -c /home/joshua/sdr.conf -r 22050 -D 1 -B 1200 -

