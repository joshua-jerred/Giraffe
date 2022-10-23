var done = true;
var timer = null;

function start() {
    document.getElementById("connection-status").innerText = "CONNECTING";
    done = false;
    pullStatic();
    pullDynamic();
}

function stop() {
    POST("DISCONNECT");
    document.getElementById("connection-status").innerText = "DISCONNECTED";
    done = true;
    clearTimeout(timer);
}

function pullStatic() {
    fetch('static-data.json', { headers: {'Cache-Control': 'no-cache'}})
        .then(function (response) {
            return response.json();
        })
        .then(function (data) {
            updateStatic(data);
            document.getElementById("connection-status").innerText = "CONNECTED";
        })
        .catch(function (err) {
            console.log('error: ' + err);
        });
}

function pullDynamic() {
    if (done) return;
    fetch('dynamic-data.json', { headers: {'Cache-Control': 'no-cache'}})
        .then(function (response) {
            return response.json();
        })
        .then(function (data) {
            updateDynamic(data);
        })
        .catch(function (err) {
            console.log('error: ' + err);
        });
    timer = setTimeout(pullDynamic, 500);
}

function updateStatic(data) {
    let staticData = document.getElementById('static-data');
    staticData.innerHTML = '';
    for (let key in data) {
        if (key == "extensions") {
            for (let ext in data[key]) {
                staticData.innerHTML += ext + ': ' + data[key][ext]['name'] + '<br>';
            }
        } else {
            let value = data[key];
            let div = document.createElement('div');
            div.innerHTML = key + ': ' + value;
            staticData.appendChild(div);
        }
    }
}

function updateDynamic(data) {
    let staticData = document.getElementById('dynamic-data');
    staticData.innerHTML = '';
    for (let key in data) {
        let value = data[key];
        let div = document.createElement('div');
        div.innerHTML = key + ': ' + value;
        staticData.appendChild(div);
    }
}

function shutdownServer() {
    document.getElementById("connection-status").innerText = "SERVER SHUT DOWN REQUESTED";
    done = true;
    POST("shutdownServer");
}

function shutdownGFS() {
    done = true;
    document.getElementById("connection-status").innerText = "GFS SHUT DOWN REQUESTED";
    POST("shutdownGFS");
}

async function POST(command) {
    var xhr = new XMLHttpRequest();
    xhr.open("POST", "/post", true);
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.send(JSON.stringify({
        "command": command
    }));
}