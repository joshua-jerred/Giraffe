#!/bin/bash

set -e
set -u

mkdir -p /opt/giraffe
mkdir -p /var/lib/giraffe

# Install the binary
cp ./flight_system/gfs /opt/giraffe/gfs
chmod +x /opt/giraffe/gfs

# Check if the service file already exists
if [ -f /etc/systemd/system/gfs.service ]; then
    echo "Service file already exists. Update? [y/N]"
    read -r response
    if [[ ! "$response" =~ ^[Yy]$ ]]; then
        echo "Skipping service file installation."
        exit 0
    fi

    systemctl stop gfs.service || true
    systemctl disable gfs.service || true
    rm -f /etc/systemd/system/gfs.service
fi

# Install the service file
cp ./flight_system/gfs.service /etc/systemd/system/gfs.service
systemctl enable gfs.service
systemctl start gfs.service