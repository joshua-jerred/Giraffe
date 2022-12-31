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

function failure() {
    document.getElementById("connection-status").innerText = "GFS CONNECTION FAILED";
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
    let extensionConfig = document.getElementById('extension-config');
    staticData.innerHTML = '';
    for (let key in data) {
        if (key == "extensions") {
            updateExtensionTable(data[key]);
        } else {
            let value = data[key];
            let div = document.createElement('div');
            div.innerHTML = key + ': ' + value;
            staticData.appendChild(div);
        }
    }
}

function updateExtensionTable(data) { // https://www.valentinog.com/blog/html-table/ - source for this
    let table = document.getElementById('extension-config');
    let table_header = table.createTHead();
    let row = table_header.insertRow();
    let keys = Object.keys(data[0]);
    for (let element of data) {
        let row = table.insertRow();
        for (key in element) {
            let cell = row.insertCell();
            let text = document.createTextNode(element[key]);
            cell.appendChild(text);
        }
    }
}

function updateDynamic(data) {
    if (data['dynamic'] == 'failed') {
        failure();
        return;
    }

    let dynamic = data['dynamic'];
    let tx_queue = data['tx-queue'];
    let extension_status = data['extension-status'];

    let html_div = document.getElementById('dynamic-data');
    html_div.innerHTML = '';
    for (let key in dynamic) {
        let source = dynamic[key]['source'];
        let name = dynamic[key]['unit'];
        let value = dynamic[key]['value'];
        let div = document.createElement('div');
        div.innerHTML = source + '-' + name + ': ' + value;
        html_div.appendChild(div);
    }

    let tx_queue_div = document.getElementById('tx-queue');
    tx_queue_div.innerHTML = '';
    for (let key in tx_queue) {
        let file = tx_queue[key]['file'];
        let type = tx_queue[key]['type'];
        let duration = tx_queue[key]['duration'];
        let div = document.createElement('div');
        div.innerHTML = file + ': ' + type + ' ' + duration + 's';
        tx_queue_div.appendChild(div);
    }

    let extension_status_div = document.getElementById('extension-status');
    extension_status_div.innerHTML = '';
    for (let key in extension_status) {
        let div = document.createElement('div');
        div.innerHTML = key + ': ' + extension_status[key];
        extension_status_div.appendChild(div);
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