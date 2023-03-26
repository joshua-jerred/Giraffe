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

function navMenuDropdown(container_name) {
    console.log("oof");
    var x = document.getElementById(container_name);
    if (x.className.indexOf("w3-show") == -1) {
        x.className += " w3-show";
    } else {
        x.className = x.className.replace(" w3-show", "");
    }
}

window.addEventListener("DOMContentLoaded", function () {
    UtcClock();
    UpdateGwsStatus();

    this.setInterval(UpdateGwsStatus, 1000);

    document.querySelector('#nav-gfs-dropdown-button')
        .addEventListener('click', function () {
            navMenuDropdown('nav-gfs-dropdown')
        });

    document.querySelector('#nav-ggs-dropdown-button')
        .addEventListener('click', function () {
            navMenuDropdown('nav-ggs-dropdown')
        });

});