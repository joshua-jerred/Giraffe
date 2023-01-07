/**
 * @file data-display.js
 * @author Joshua Jerred (github.com/joshua-jerred)
 * @brief Javascript for the web server
 * @details I am not a web developer; this is my first real attempt at JS. 
 * I apologize to anyone who looks at this.
 */

var done = true;
var timer = null;
var telem_timer = null;

function start() {
    document.getElementById("connection-status").innerText = "CONNECTING";
    done = false;
    pullStatic();
    pullDynamic();
    pullTelemetry();
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

function pullTelemetry() {
    if (done) return;
    fetch('telemetry-data.json', { headers: {'Cache-Control': 'no-cache'}})
        .then(function (response) {
            return response.json();
        })
        .then(function (data) {
            updateTelemetry(data);
        })
        .catch(function (err) {
            console.log('error: ' + err);
    });
    telem_timer = setTimeout(pullTelemetry, 1000);
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

    let errors_div = document.getElementById('error-log');
    errors_div.innerHTML = '';
    for (let key in data['errors']) {
        let div = document.createElement('div');
        let source = data['errors'][key]['source'];
        let code = data['errors'][key]['code'];
        let info = data['errors'][key]['info'];
        div.innerHTML = source + ': ' + code + ' ' + info;
        errors_div.appendChild(div);
    }
}

function updateTelemetry(data) {
    if (data['telemetry'] == 'failed') {
        failure();
        return;
    }
    let info = data['tx-log-info'];
    let size = info['size'];
    let max_size = info['max_size'];
    let first = info['first'];
    let last = info['last'];
    updateTxLogStats(size, max_size, first, last);

    removeLogsFromFirstToLast(first, last);
    let log = data['tx-log'];
    let log_div = document.querySelector('#telemetry-log');
    for (let key in log) {
        let id = log[key]['ID'];
        // Check if the log is already in the list
        if (log_div.querySelector('#txid_' + id) != null) continue;

        let type = log[key]['type'];
        let length = log[key]['length'];
        let message = log[key]['message'];

        addTxToLog(id, type, length, message);
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

function updateTxLogStats(size, max_size, first, last) {
    let stats = document.querySelector('#tx-log-stats');
    stats.querySelector('#tx-log-size').innerText = size;
    stats.querySelector('#tx-log-max-size').innerText = max_size;
    stats.querySelector('#tx-log-first-id').innerText = first;
    stats.querySelector('#tx-log-last-id').innerText = last;
}

function addTxToLog(id, type, length, message) {
    // Copy the template for the log item
    let log_item_template = document.querySelector('#tx-log-item-template');
    let new_log_item = log_item_template.cloneNode(true);
    new_log_item.style.display = 'block';

    // Set the id of the new log item to the tx id
    new_log_item.id = 'txid_' + id;
    let info_div = new_log_item.querySelector('.tx-log-info');
    let message_div = new_log_item.querySelector('.tx-log-message');

    // Fill the info contents
    let tx_id = info_div.querySelector('.tx-log-id');
    tx_id.innerText = id;
    let tx_type = info_div.querySelector('.tx-log-type');
    tx_type.innerText = type;
    let tx_length = info_div.querySelector('.tx-log-length');
    tx_length.innerText = length;

    // Fill the message contents
    message_div.innerText = message;

    let tx_log = document.querySelector('#telemetry-log');
    tx_log.append(new_log_item);
}

function removeLogsFromFirstToLast(first, last) {
    let tx_log = document.querySelector('#telemetry-log');
    let log_items = tx_log.querySelectorAll('.tx-log-item');
    for (let log_item of log_items) {
        let id = log_item.id.split('_',).pop();
        if (id < first || id > last) {
            log_item.remove();
        }
    }
}

function clearTxLog() {
    let tx_log = document.querySelector('#telemetry-log');
    tx_log.innerHTML = '';
}

function retransmit(e) {
    if (e.preventDefault) e.preventDefault();
    let id = document.getElementById('request-txid').value;
    POST("cmd/tlm/rtx/" + id);
    return false;
}

window.addEventListener("DOMContentLoaded", function() {
  document.getElementById('retx').addEventListener('submit', retransmit);
});