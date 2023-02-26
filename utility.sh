#!/usr/bin/env bash

# A simple utility for managing the Giraffe Flight Computer during development

# Options: 
# full-sync (Syncs, builds, installs, and restarts the services)
# Service options:
# sync-services, start-services, stop-services     -- BOTH SERVICES
# start-gfs, stop-gfs, status-gfs, service-log-gfs -- GFS
# start-web, stop-web, status-web, service-log-web -- WEB SERVER
# sync (sync src, tests, CMakeLists.txt)
# build, install
# --- Web Server ---
# set-permissions
# sync-web-server

SSH_HOST="balloon"
GFS_USER="giraffe"

GFS_WEB_SERVICE="gfs-web-server"
GFS_SERVICE="gfs-watchdog"
REQUEST=$1

# Check if the host is up
uptime=$(ssh $SSH_HOST "uptime" 2>/dev/null | grep up)
if [ "${#uptime}" -gt 20 ]; then
    echo "Host is up"
else
    echo "Host is down"
    exit 1
fi

syncronize_src() {
    echo "Syncing"
    rsync -v -r src/ balloon:Giraffe/src
    rsync -v -r tests/ balloon:Giraffe/tests
    rsync -v -r CMakeLists.txt balloon:Giraffe/CMakeLists.txt
}

start_services() {
    echo "Starting services"
    ssh $SSH_HOST "sudo systemctl start $GFS_WEB_SERVICE"
    ssh $SSH_HOST "sudo systemctl start $GFS_SERVICE"
}

stop_services() {
    echo "Stopping services"
    ssh $SSH_HOST "sudo systemctl stop $GFS_WEB_SERVICE"
    ssh $SSH_HOST "sudo systemctl stop $GFS_SERVICE"
}

build() {
    echo "Building"
    ssh $SSH_HOST "cd Giraffe && mkdir -p build && cd build && cmake .. && make giraffe-flight-software"
}

install() {
    echo "Installing"
    ssh $SSH_HOST "cd Giraffe/build && sudo make install giraffe-flight-software"
    ssh $SSH_HOST "sudo chown -R $GFS_USER:$GFS_USER /opt/giraffe"
}

set_permissions() {
    echo "Setting permissions"
    ssh $SSH_HOST "sudo chown -R $GFS_USER:$GFS_USER /opt/giraffe"
    ssh $SSH_HOST "sudo chmod -R 774 /opt/giraffe"
}

sync_web_server() {
    echo "Syncing web server"
    #ssh $SSH_HOST "sudo systemctl stop $GFS_WEB_SERVICE"
    rsync -v -r --rsync-path="sudo rsync" src/flight-computer/web-server/ $SSH_HOST:/opt/giraffe/web-server
    #ssh $SSH_HOST "sudo systemctl start $GFS_WEB_SERVICE"
}

if [ "$REQUEST" == "start-web" ]; then
    echo "Starting web server"
    ssh $SSH_HOST "sudo systemctl start $GFS_WEB_SERVICE"
elif [ "$REQUEST" == "stop-web" ]; then
    echo "Stopping web server"
    ssh $SSH_HOST "sudo systemctl stop $GFS_WEB_SERVICE"
elif [ "$REQUEST" == "start-gfs" ]; then
    echo "Starting gfs"
    ssh $SSH_HOST "sudo systemctl start $GFS_SERVICE"
elif [ "$REQUEST" == "stop-gfs" ]; then
    echo "Stopping gfs"
    ssh $SSH_HOST "sudo systemctl stop $GFS_SERVICE"
elif [ "$REQUEST" == "sync-services" ]; then
    echo "Syncing services"
    rsync -v -r --rsync-path="sudo rsync" src/linux-services/gfs-web-server.service $SSH_HOST:/etc/systemd/system/gfs-web-server.service
    rsync -v -r --rsync-path="sudo rsync" src/linux-services/gfs-watchdog.service $SSH_HOST:/etc/systemd/system/gfs-watchdog.service
    ssh $SSH_HOST "sudo systemctl daemon-reload"
elif [ "$REQUEST" == "service-log-web" ]; then
    echo "Web service log"
    ssh $SSH_HOST "journalctl -n 30 -u $GFS_WEB_SERVICE"
elif [ "$REQUEST" == "service-log-gfs" ]; then
    echo "GFS service log"
    ssh $SSH_HOST "journalctl -n 30 -u $GFS_SERVICE"
elif [ "$REQUEST" == "status-web" ]; then
    echo "Web service status"
    ssh $SSH_HOST "systemctl status $GFS_WEB_SERVICE"
elif [ "$REQUEST" == "status-gfs" ]; then
    echo "GFS service status"
    ssh $SSH_HOST "systemctl status $GFS_SERVICE"
elif [ "$REQUEST" == "sync" ]; then
    syncronize_src
elif [ "$REQUEST" == "build" ]; then
    build
elif [ "$REQUEST" == "install" ]; then
    install
elif [ "$REQUEST" == "start-services" ]; then
    start_services
elif [ "$REQUEST" == "stop-services" ]; then
    stop_services
elif [ "$REQUEST" == "set-permissions" ]; then
    set_permissions
elif [ "$REQUEST" == "full-sync" ]; then
    echo "Full sync"
    stop_services
    syncronize_src
    build
    install
    set_permissions
    start_services
elif [ "$REQUEST" == "sync-web-server" ]; then
    sync_web_server
    set_permissions
else
    echo "Unknown request"
    exit 1
fi

# Check if the service exists
web_service=$(ssh $SSH_HOST "systemctl status $GFS_WEB_SERVICE" 2>&1)
case $web_service in
    *"could not be found"*)
        echo "Web service does not exist"
        exit 1
        ;;
    *"Active: active"*)
        echo "Web service is active"
        ;;
    *"Active: inactive"*)
        echo "Web service is inactive"
        ;;
    *) echo "Unknown web service status"
        exit 1
        ;;
esac

# Check if the service exists
gfs_service=$(ssh $SSH_HOST "systemctl status $GFS_SERVICE" 2>&1)
case $gfs_service in
    *"could not be found"*)
        echo "GFS service does not exist"
        exit 1
        ;;
    *"Active: active"*)
        echo "GFS service is active"
        ;;
    *"Active: inactive"*)
        echo "GFS service is inactive"
        ;;
    *) echo "Unknown GFS service status"
        exit 1
        ;;
esac