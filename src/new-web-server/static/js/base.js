var mode = "error"; // gfs, ggs, or error
var gws_status = false;
var gfs_status = false;
var ggs_status = false;

const status_event = new Event("StatusUpdate");

async function UtcClock() {
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
    setTimeout(UtcClock, 1000);
}

function UpdatedModeIndicator() {
    let gfs_mode_indicator = document.querySelector('#nav-gfs-mode');
    let ggs_mode = document.querySelector('#nav-ggs-mode');
    let warning_text = document.querySelector('#nav-warning');

    let error_color = getComputedStyle(document.documentElement)
        .getPropertyValue('--error');
    let good_color = getComputedStyle(document.documentElement)
        .getPropertyValue('--good');

    if (mode != "error") {
        if (gfs_status) {
            gfs_mode_indicator.style.borderColor = good_color;
        } else {
            gfs_mode_indicator.style.borderColor = error_color;
        }
        if (ggs_status) {
            ggs_mode.style.borderColor = good_color;
        } else {
            ggs_mode.style.borderColor = error_color;
        }
    }

    if (mode == "gfs") {
        gfs_mode_indicator.style.opacity = 1;
        ggs_mode.style.opacity = 0.5;
        if (!gfs_status) {
            warning_text.style.display = "inline";
            warning_text.innerText = "GFS not connected";
        } else {
            warning_text.style.display = "none";
        }
    } else if (mode == "ggs") {
        gfs_mode_indicator.style.opacity = 0.5;
        ggs_mode.style.opacity = 1;
        if (!ggs_status) {
            warning_text.style.display = "inline";
            warning_text.innerText = "GGS not connected";
        } else {
            warning_text.style.display = "none";
        }
    } else {
        gfs_mode_indicator.style.opacity = 0.5;
        ggs_mode.style.opacity = 0.5;
        gfs_mode_indicator.style.borderColor = error_color;
        ggs_mode.style.borderColor = error_color;
        warning_text.style.display = "inline";
        warning_text.innerText = "GWS not connected";
    }
}

async function UpdateLocalMode() {
    let nav_button_display = "grid";

    let connected = false;

    SetModeSwitch();
    UpdatedModeIndicator();

    let gfs_mode_buttons = document.querySelector('#nav-mode-buttons-gfs');
    let ggs_mode_buttons = document.querySelector('#nav-mode-buttons-ggs');

    if (mode == "gfs") {
        connected = true;
        gfs_mode_buttons.classList.remove("inactive_mode");
        gfs_mode_buttons.classList.add("active_mode");

        ggs_mode_buttons.classList.remove("active_mode");
        ggs_mode_buttons.classList.add("inactive_mode");
    } else if (mode == "ggs") {
        connected = true;
        gfs_mode_buttons.classList.remove("active_mode");
        gfs_mode_buttons.classList.add("inactive_mode");

        ggs_mode_buttons.classList.remove("inactive_mode");
        ggs_mode_buttons.classList.add("active_mode");
    } else {
        gfs_mode_buttons.classList.remove("active_mode");
        gfs_mode_buttons.classList.add("inactive_mode");

        ggs_mode_buttons.classList.remove("active_mode");
        ggs_mode_buttons.classList.add("inactive_mode");
    }

    document.querySelectorAll('.inactive_mode').forEach((element) => {
        element.style.display = "none";
    });

    document.querySelectorAll('.active_mode').forEach((element) => {
        element.style.display = nav_button_display;
    });

    let status_elm = document.querySelector('#nav-connection-status')
    if (!connected) {
        status_elm.innerText = "Disconnected";
        status_elm.style.color = "red";
        gws_status = false;
        gfs_status = false;
        ggs_status = false;
    } else {
        gws_status = true;
        status_elm.innerText = "Connected";
        status_elm.style.color = "green";
    }

    document.dispatchEvent(status_event);
}

async function UpdateGwsStatus() {
    fetch("/gws/status", {
        method: "GET",
        headers: {
            "Content-Type": "application/json",
        }
    })
    .then(response => response.json())
    .then((response) => {
        if (response["gws-status"] != "ok") {
            mode = "error";
            UpdateLocalMode();
        } else {
            let new_mode = response["mode"];
            let new_gfs_status = response["gfs-connected"];
            let new_ggs_status = response["ggs-connected"];
            if (new_mode != mode || new_gfs_status != gfs_status
                || new_ggs_status != ggs_status) {
                mode = new_mode;
                gfs_status = new_gfs_status;
                ggs_status = new_ggs_status;
                UpdateLocalMode();
            }
        }
    }).catch((error) => {
        console.log(error);
        mode = "error";
        UpdateLocalMode();
    });
}

function SetModeSwitch() {
    let mode_switch = document.querySelector('#mode-check-box');
    mode_switch.checked = mode == "ggs";
}

function NotImplemented() {
    alert("this literally does nothing");
}

async function ModeSwitch() {
    let mode_switch = document.querySelector('#mode-check-box');
    let new_mode = mode_switch.checked ? "gfs" : "ggs";
    fetch("/gws/mode", {
        method: "POST",
        body: JSON.stringify({
            "mode": new_mode
        }),
        headers: {
            "Content-Type": "application/json",
        }
    })
        .then((response) => {
            if (response.status != 200) {
                console.log("Error Switching Modes: " + response.status);
            }
            UpdateGwsStatus();
        }).catch((error) => {
            console.log("Error Switching Modes: " + error);
            SetModeSwitch();
        });
}

window.addEventListener("DOMContentLoaded", function () {
    UtcClock();
    UpdateGwsStatus();

    this.setInterval(UpdateGwsStatus, 1000);

    document.querySelector('#mode-switch')
        .addEventListener('click', ModeSwitch);
});