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
    if (connection_status) {
        await fetch('/api/disconnect');
        updateConnectionStatus();
    } else {
        await fetch('/api/connect');
        updateConnectionStatus();
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
    } else {
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

window.addEventListener("DOMContentLoaded", function() {
    updateConnectionStatus()

    let coll = document.getElementsByClassName("content-box-title");
    let i;
    
    for (i = 0; i < coll.length; i++) {
      coll[i].addEventListener("click", function() {
        this.classList.toggle("active");
        var title_arrow = this.children[0];
        var content = this.nextElementSibling;
        if (content.style.display != "none") {
            content.style.display = "none";
            title_arrow.innerText = "\u2B9C";
        } else {
          content.style.display = "block";
            title_arrow.innerText = "\u2B9F";
        }
      });
    }
});

