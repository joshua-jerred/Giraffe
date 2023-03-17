var connection_requested = false;
var connection_status = false;
var connection_start = 0;

async function updateConnectionTime() {
    let connectionTime = document.querySelector('#nav-connection-uptime');
    if (!connection_status) {
        connectionTime.innerText = "--:--";
        connection_start = 0;
        return;
    }
    if (connection_start == 0) {
        let connectionTimeValue = await fetch('/api/get-connection-uptime')
                                        .then((response) => response.json())
                                        .then((data) => {return data["uptime"]});
        let t = new Date();
        connection_start = Math.floor(t.getTime()/1000) - connectionTimeValue;
    }

    let t = new Date();
    let connectionTimeValue = Math.floor(t.getTime()/1000) - connection_start;

    connectionTime.innerText = getStringTime(connectionTimeValue);
    setTimeout(updateConnectionTime, 1000);
}


function getStringTime(time) {
    if (isNaN(time)) {
        return "--:--:--";
    } else if (time < 0) {
        return "--:--:--";
    } else if (time == 0) {
        return "00:00:00";
    }

    let seconds = Math.floor(time % 60);
    let minutes = Math.floor((time / 60) % 60);
    let hours = Math.floor((time / 3600) % 24);
    let days = Math.floor(time / 86400);

    let timeString = "";
    if (days > 0) {
        timeString += days + ":";
    }

    if (hours > 0) {
        if (hours < 10) {
            timeString += hours = "0" + hours + ":";
        } else {
            timeString += hours + ":";
        }
    }
    
    if (minutes == 0) {
        timeString += "00:";
    } else if (minutes < 10) {
        timeString += "0" + minutes + ":";
    } else {
        timeString += minutes + ":";
    }

    if (seconds < 10) {
        timeString += "0" + seconds;
    } else {
        timeString += seconds;
    }

    return timeString;
}

async function toggleConnection() {
    if (connection_requested) { // Disconnect
        await fetch('/api/disconnect');
        connection_requested = false;
        updateConnectionStatus();
    } else { // Connect
        connection_requested = true;
        await connect();
    }
}

async function connect() {
    while (connection_requested && !connection_status) {
        console.log("Attempting to connect...");
        await fetch('/api/connect');
        document.title = "GFS - Connecting...";
        document.querySelector('#nav-connection-button').innerText = "Cancel";
        document.querySelector('#nav-connection-status').innerText = "Connecting...";
        updateConnectionStatus();
        await new Promise(r => setTimeout(r, 1000));
    }
}


async function updateConnectionStatus() {
    let connected = await fetch('/api/get-connection-status')
        .then((response) => response.json())
        .then((data) => {return data["connected"]})
        .catch((error) => {return false});

    let connectionStatus = document.querySelector('#nav-connection-status');
    let connectionButton = document.querySelector('#nav-connection-button');
    let gfs_control_buttons = document.getElementsByClassName('gfs-control-button');
    if (connected) {
        connection_status = true;
        document.title = "GFS - Connected";
        connectionButton.innerText = "Disconnect";
        connectionStatus.innerText = "Connected";
        for (var i = 0; i < gfs_control_buttons.length; i++) {
            gfs_control_buttons[i].style.display = "inline-block";
        }
        updateConnectionTime();
    } else if (!connection_requested) {
        connection_status = false;
        document.title = "GFS - Disconnected";
        connectionButton.innerText = "Connect";
        connectionStatus.innerText = "Disconnected";
        for (var i = 0; i < gfs_control_buttons.length; i++) {
            gfs_control_buttons[i].style.display = "none";
        }
    }
}

async function uptime() {
    let uptime = await fetch('/api/uptime')
        .then((response) => response.json())
        .then((data) => {return data["uptime"]});

    let uptimeElement = document.querySelector('#uptime');
    uptimeElement.innerText = uptime;
}

async function getConnectionStatus() {
    let connection_data = await fetch('/api/get-connection-status')
        .then((response) => response.json())
        .then((data) => {return data})
        .catch((error) => {return false});
    return connection_data["connected"];
}

async function utcClock() {
    let date = new Date();
    let hours = date.getUTCHours();
    if (hours < 10) {
        hours = "0" + hours;
    }
    let minutes = date.getUTCMinutes();
    if (minutes < 10) {
        minutes = "0" + minutes;
    }
    let seconds = date.getUTCSeconds();
    if (seconds < 10) {
        seconds = "0" + seconds;
    }
    let time = hours + ":" + minutes + ":" + seconds;
    document.querySelector('#utc-clock').innerText = time;
    setTimeout(utcClock, 1000);
}

async function connection_handler_runner() {
    connection_requested = await fetch('/api/get-connection-requested')
    .then((response) => response.json())
    .then((data) => {
        return data["connection_requested"]})
        .catch((error) => {return false});
    
    if (connection_requested) {    
        connection_status = await getConnectionStatus();
        
        if (connection_requested && !connection_status) {
            document.querySelector('#nav-connection-button').innerText = "Cancel";
            document.querySelector('#nav-connection-status').innerText = "Connecting...";
            await connect();
        } else if (connection_requested && connection_status) {
            updateConnectionStatus();
        } else {
            console.log("Connection not requested");
        }
    }
    setTimeout(connection_handler_runner, 2000);
}

async function sendCommand(command) {
    console.log(command);
    fetch('/api/command', {
        method: 'POST',
        headers: {
            'Accept' : 'application/json',
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({"command": command})
    })
    .then((response) => response.json())
    .then((data) => {
        console.log(data);
    })
    .catch((error) => {
        console.log(error);
    });
}

async function commandButtonHandler() {
    let command = document.getElementById('command-input').value;
    sendCommand(command);
}

window.addEventListener("DOMContentLoaded", function() {
    utcClock();
    connection_handler_runner();

    let commandButton = document.getElementById('command-button');
    commandButton.addEventListener("click", commandButtonHandler);

    let coll = document.getElementsByClassName("content-box-title");
    let i;
    
    for (i = 0; i < coll.length; i++) {
      coll[i].addEventListener("click", function() {
        this.classList.toggle("active");
        var title_arrow = this.children[0];
        var content = this.nextElementSibling;
        if (content.style.display != "none") {
            content.style.display = "none";
            title_arrow.innerText = "\u25B2"; // hide, up arrow
        } else {
          content.style.display = "block";
            title_arrow.innerText = "\u25BC"; // show, down arrow
        }
      });
    }
});

