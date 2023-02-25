#!/bin/bash
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run with root privileges." 
   exit 1
fi

# Check if the 'giraffe' user exists
if id -u "giraffe" >/dev/null 2>&1; then
  echo "The 'giraffe' user exists. Continuing..."
else
  echo "The 'giraffe' user does not exists. Please create is with a home directory."
  echo "sudo useradd -m giraffe"
  exit 1
fi

read -p "Do you want to install the flight software prerequisites?: " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]
then
  echo "Installing flight software prerequisites..."
  apt-get update
  apt-get install -y libi2c-dev i2c-tools python3-pip build-essential cmake
  pip3 install flask
fi

# Install the web server service
read -p "Do you want to install the web server service?: " -n 1 -r
echo    
if [[ $REPLY =~ ^[Yy]$ ]]
then
	echo "Installing web server service..."
	cp gfs-web-server.service /etc/systemd/system/
  chmod 644 /etc/systemd/system/gfs-web-server.service
  chown root:root /etc/systemd/system/gfs-web-server.service
  chmod 755 /opt/giraffe/web-server/main.py
  chown giraffe:giraffe /opt/giraffe/web-server/main.py
	systemctl daemon-reload
	systemctl start gfs-web-server
fi