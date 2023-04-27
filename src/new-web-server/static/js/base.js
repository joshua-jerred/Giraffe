var gws_status = false;
var gfs_status = false;
var ggs_status = false;

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
            } else {
                gfs_status = response["gfs-connected"];
                ggs_status = response["ggs-connected"];
            }
        }).catch((error) => {
            console.log(error);
        });
}

function navButtonToggle(selector, arrow_selector) {
    let arrow = document.querySelector(arrow_selector);
    let buttons = document.querySelectorAll('.nav-button' + selector);
    previous_display = buttons[0].style.display;
    if (previous_display == "none") {
        buttons.forEach(function (button) {
            button.style.display = "";
        });
        arrow.innerText = '\u2B9E';
    } else {
        buttons.forEach(function (button) {
            button.style.display = "none";
        });
        arrow.innerText = '\u2B9C';
    }
}

window.addEventListener("DOMContentLoaded", function () {
    UtcClock();
    UpdateGwsStatus();

    this.setInterval(UpdateGwsStatus, 1000);

    document.querySelector('#nav-gfs')
        .addEventListener('click', function () {
            navButtonToggle('.gfs-color', '#gfs-expansion-arrow')
        });

    document.querySelector('#nav-ggs')
        .addEventListener('click', function () {
            navButtonToggle('.ggs-color', '#ggs-expansion-arrow')
        });

});