function GetStringTime(time) {
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

// <div class="collapsible-box">
//  <div class="collapsible-title">Title</div>
//    <div class="collapsible-content" onclick>
//          Content
//    </div>
// </div>
function Collapsible(content) {
    if (content.style.display != "none") {
        content.style.display = "none";
    } else {
        content.style.display = "block";
    }
}

window.addEventListener("DOMContentLoaded", function () {
    let collapsibles = document.querySelectorAll(".collapsible-box");
    if (collapsibles.length == 0) {
        return;
    }
    for (let i = 0; i < collapsibles.length; i++) {
        let box_title = collapsibles[i].querySelector(".collapsible-title");
        let box_content = collapsibles[i].querySelector(".collapsible-content");

        box_title.addEventListener("click", function () {
            Collapsible(box_content);
        });

    }
});