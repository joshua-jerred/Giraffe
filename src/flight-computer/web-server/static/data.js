var config_loaded = false;

async function updateConfig() {
    let connected = await getConnectionStatus();
    if (connected) {
        let config = await fetch('/api/get-config')
            .then((response) => response.json())
            .then((data) => {return data})
            .catch((error) => {return false});
        
        config_loaded = false;
        if (!config.hasOwnProperty("general")) {
            console.log("No general config.");
            return;
        }
        if (!config.hasOwnProperty("extensions")) {
            console.log("No extensions config.");
            return;
        }
        if (!config.hasOwnProperty("debugging")) {
            console.log("No debugging config.");
            return;
        }
        if (!config.hasOwnProperty("telemetry")) {
            console.log("No telemetry config.");
            return;
        }
        if (!config.hasOwnProperty("data-log-data-and-packet-contents")) {
            console.log("No data log config.");
            return;
        }
        if (!config.hasOwnProperty("flight-procs")) {
            console.log("No flight procs config.");
            return;
        }
        config_loaded = true;

        parseGeneralConfig(config["general"]);
        parseExtensionsConfig(config["extensions"]);
        parseDebuggingConfig(config["debugging"]);
        parseTelemetryConfig(config["telemetry"]);
        //parseDataLogConfig(config["data-log-data-and-packet-contents"]);
        //parseFlightProcsConfig(config["flight-procs"]);
        console.log("Config loaded.");
        return;
    } else {
        setTimeout(updateConfig, 200);
    }
}

async function parseGeneralConfig(config) {
    try {
        document.getElementById("cfg-gfs-version").innerHTML = config["gfs-version"];
        document.getElementById("cfg-project-name").innerHTML = config["project-name"];
        document.getElementById("cfg-main-board-type").innerHTML = config["main-board-type"];
        document.getElementById("cfg-starting-procedure").innerHTML = config["starting-procedure"];
    } catch (error) {
        console.log("Error parsing general config.");
        console.log(error);
    }
}

async function parseExtensionsConfig(config) {
    let total = 0
    let GPS = 0;
    let IMU = 0;
    let radio = 0;
    let externalSensors = 0;
    let internalSensors = 0;
    let other = 0;
    try {
        for (const ext of config) {
            total++;
            let id = ext["id"];
            let name = ext["name"];
            let type = ext["type"];
            let category = ext["category"];
            if (category == "gps") {
                GPS++;
            } else if (category == "imu") {
                IMU++;
            } else if (category == "radio") {
                radio++;
            } else if (category == "external_sensor") {
                externalSensors++;
            } else if (category == "internal_sensor") {
                internalSensors++;
            } else {
                other++;
            }
            let interface = ext["interface"];
            let update_interval = ext["interval"];
            let flight_critical = ext["critical"];
            let extra_args = ext["extra-args"];

            let new_extension = document.querySelector("#ext_template").cloneNode(true);
            new_extension.style.display = "table-row";
            new_extension.id = "ext_" + name;
            new_extension.querySelector(".e_id").innerHTML = id;
            new_extension.querySelector(".e_name").innerHTML = name;
            new_extension.querySelector(".e_type").innerHTML = type;
            new_extension.querySelector(".e_category").innerHTML = category;
            new_extension.querySelector(".e_interface").innerHTML = interface;
            new_extension.querySelector(".e_interval").innerHTML = update_interval;
            new_extension.querySelector(".e_flight-critical").innerHTML = flight_critical;
            new_extension.querySelector(".e_extra-info").innerHTML = extra_args;
            new_extension.querySelector(".e_status").innerHTML = "Unknown";
            document.querySelector("#extension-table").appendChild(new_extension);
        }
        document.getElementById("cfg-total-extensions").innerHTML = total;

        let gps_div = document.getElementById("cfg-gps-extensions");
        if (GPS == 0) {
            gps_div.innerHTML = "None";
        } else if (GPS == 1) {
            gps_div.innerHTML = "Yes";
        } else {
            gps_div.innerHTML = GPS + "x";
        }

        let imu_div = document.getElementById("cfg-imu-extensions");
        if (IMU == 0) {
            imu_div.innerHTML = "None";
        } else if (IMU == 1) {
            imu_div.innerHTML = "Yes";
        } else {
            imu_div.innerHTML = IMU + "x";
        }

        let radio_div = document.getElementById("cfg-radio-extensions");
        if (radio == 0) {
            radio_div.innerHTML = "None";
        } else if (radio == 1) {
            radio_div.innerHTML = "Yes";
        } else {
            radio_div.innerHTML = radio + "x";
        }

        let external_sensors_div = document.getElementById("cfg-external-sensors-extensions");
        if (externalSensors == 0) {
            external_sensors_div.innerHTML = "None";
        } else {
            external_sensors_div.innerHTML = externalSensors + "x";
        }

        let internal_sensors_div = document.getElementById("cfg-internal-sensors-extensions");
        if (internalSensors == 0) {
            internal_sensors_div.innerHTML = "None";
        } else {
            internal_sensors_div.innerHTML = internalSensors + "x";
        }

        document.getElementById("cfg-other-extensions").innerHTML = other + "x";
        



    } catch (error) {
        console.log("Error parsing extensions config.");
        console.log(error);
    }
}

async function parseDebuggingConfig(config){
    try {
        let console_enabled = config["console-enabled"];
        let element = document.getElementById("cfg-console-enabled");
        if (console_enabled) {
            element.innerHTML = "Enabled";
        } else {
            element.innerHTML = "Disabled";
        }
        document.getElementById("cfg-console-update-interval").innerHTML = config["console-update-interval"] + " ms";
        
        let web_server_enabled = config["web-server-enabled"];
        let enabled = document.getElementById("cfg-web-server-enabled");
        if (web_server_enabled) {
            enabled.innerHTML = "Enabled";
        } else {
            enabled.innerHTML = "Disabled";
        }
        document.getElementById("cfg-web-server-port").innerHTML = config["web-server-port"];
    } catch (error) {
        console.log("Error parsing debugging config.");
        console.log(error);
    }
}

async function parseTelemetryConfig(config) {
    try {
        let telemetry_enabled = config["telemetry-enabled"];
        let element = document.getElementById("cfg-telemetry-enabled");
        if (telemetry_enabled) {
            element.innerHTML = "Enabled";
        } else {
            element.innerHTML = "Disabled";
        }

        document.getElementById("cfg-call-sign").innerHTML = config["call-sign"];
        document.getElementById("cfg-data-packet-mode").innerHTML = config["data-packet-mode"];

        // APRS
        let aprs_enabled = config["aprs-enabled"];
        let enabled = document.getElementById("cfg-aprs-enabled");
        if (aprs_enabled) {
            enabled.innerHTML = "Enabled";
        } else {
            enabled.innerHTML = "Disabled";
        }
        document.getElementById("cfg-aprs-frequency").innerHTML = config["aprs-frequency"] + " MHz";
        document.getElementById("cfg-aprs-ssid").innerHTML = config["aprs-ssid"];
        document.getElementById("cfg-aprs-symbol").innerHTML = config["aprs-symbol"];
        document.getElementById("cfg-aprs-fi-pin").innerHTML = config["aprs-fi-pin"];
        document.getElementById("cfg-aprs-memo").innerHTML = config["aprs-memo"];

        // SSTV
        let sstv_enabled = config["sstv-enabled"];
        let sstv_enabled_element = document.getElementById("cfg-sstv-enabled");
        if (sstv_enabled) {
            sstv_enabled_element.innerHTML = "Enabled";
        } else {
            sstv_enabled_element.innerHTML = "Disabled";
        }
        document.getElementById("cfg-sstv-frequency").innerHTML = config["sstv-frequency"] + " MHz";
        document.getElementById("cfg-sstv-mode").innerHTML = config["sstv-mode"];

        // AFSK
        let afsk_enabled = config["afsk-enabled"];
        let afsk_enabled_element = document.getElementById("cfg-afsk-enabled");
        if (afsk_enabled) {
            afsk_enabled_element.innerHTML = "Enabled";
        } else {
            afsk_enabled_element.innerHTML = "Disabled";
        }
        document.getElementById("cfg-afsk-frequency").innerHTML = config["afsk-frequency"] + " MHz";

        // PSK
        let psk_enabled = config["psk-enabled"];
        let psk_enabled_element = document.getElementById("cfg-psk-enabled");
        if (psk_enabled) {
            psk_enabled_element.innerHTML = "Enabled";
        } else {
            psk_enabled_element.innerHTML = "Disabled";
        }
        document.getElementById("cfg-psk-frequency").innerHTML = config["psk-frequency"] + " MHz";
        document.getElementById("cfg-psk-mode").innerHTML = config["psk-mode"];
    } catch (error) {
        console.log("Error parsing telemetry config.");
        console.log(error);
    }
}

async function updateStatus() {
    if (connection_status) {
        let statuses = await fetch('/api/get-status-data')
        .then((response) => response.json())
        .then((data) => {return data})
        .catch((error) => {return false});

        if (statuses.hasOwnProperty("system") == false || statuses.hasOwnProperty("gfs") == false) {
            console.log("Error fetching status data.");
            return;
        }

        parseSystemStatus(statuses["system"]);
        parseGFSStatus(statuses["gfs"]);
    }
}

async function parseSystemStatus(data) {
    update("#data-system-date", data["system-date"]);
    update("#data-system-time-utc", data["system-time-utc"]);
    update("#data-system-uptime", data["system-uptime"]);
    update("#data-cpu-load-average", data["cpu-load-average"]);

    update("#data-cpu-temp", data["cpu-temp"]);
    update("#data-memory-usage", data["memory-usage"]);
    update("#data-storage-usage", data["storage-usage"]);
    update("#data-battery-voltage", data["battery-voltage"]);
}

async function parseGFSStatus(data) {
    update("#data-gfs-uptime", data["gfs-uptime"]);
    update("#data-health-status", data["health-status"]);
    update("#data-reported-errors", data["reported-errors"]);
    update("#data-current-flight-proc", data["current-flight-proc"]);
    update("#data-flight-phase", data["flight-phase"]);
    
    let modules = data["modules-status"];
    update("#module-status-config", modules["configuration"]);
    update("#module-status-data", modules["data"]);
    update("#module-status-extension", modules["extensions"]);
    update("#module-status-telemetry", modules["telemetry"]);
    update("#module-status-console", modules["console"]);
    update("#module-status-server", modules["server"]);
}

async function updateGfsData() {
    if (connection_status) {
        let gfs_data = await fetch('/api/get-gfs-data')
        .then((response) => response.json())
        .then((data) => {return data})
        .catch((error) => {return false});
        if (!gfs_data.hasOwnProperty("data-stream")) {
            console.log("Error fetching GFS data.");
            return;
        }
        parseDataStreamStats(gfs_data["data-stream"]);
        parseCriticalData(gfs_data["critical-data"]);
        parseDataLogStats(gfs_data["data-log"]);
        parsePositionData(gfs_data["position-data"]);
        parseEnvironmentalData(gfs_data["environmental-data"]);
        parseCalculatedData(gfs_data["calculated-data"]);
    }
}

function parseDataStreamStats(data) {
    update("#data-total-data-packets", data["total-data-packets"]);
    update("#data-current-data-packets", data["current-data-packets"]);
    update("#data-total-gps-packets", data["total-gps-packets"]);
    update("#data-current-gps-packets", data["current-gps-packets"]);
    update("#data-total-error-packets", data["total-error-packets"]);
    update("#data-current-error-packets", data["current-error-packets"]);
}

function parseCriticalData(data) {
    update("#data-critical-flight-phase", data["flight-phase"]);
    update("#data-critical-gps-data-valid", data["gps-data-valid"]);
    update("#data-critical-gps-fix", data["gps-fix"]);
    update("#data-critical-gps-alt", data["gps-alt"]);
    update("#data-critical-gps-lat", data["gps-lat"]);
    update("#data-critical-gps-lon", data["gps-lon"]);
    update("#data-critical-pressure-data-valid", data["pressure-data-valid"]);
    update("#data-critical-pressure-mbar", data["pressure-mbar"]);
    update("#data-critical-battery-data-valid", data["battery-data-valid"]);
    update("#data-critical-battery-voltage", data["battery-voltage"]);
    update("#data-critical-system-data-valid", data["system-data-valid"]);
    update("#data-critical-ram-usage", data["ram-usage"].toFixed(2));
    update("#data-critical-disk-usage", data["disk-usage"].toFixed(2));
    update("#data-critical-radio-status", data["radio-status"]);
}

function parseDataLogStats(data) {
    update("#data-log-enabled", data["enabled"]);
    update("#data-log-file-name", data["file-name"]);
    update("#data-log-file-size", data["file-size-mb"] + " mb");
}

function parsePositionData(data) {
    update("#data-gps-source-extension", data["source"]);
    update("#data-gps-fix", data["fix"]);
    update("#data-gps-time-stamp", data["time"]);
    update("#data-gps-lat", data["latitude"]);
    update("#data-gps-lon", data["longitude"]);
    update("#data-gps-alt", data["altitude"]);
    update("#data-gps-v-speed", data["vertical-speed"]);
    update("#data-gps-ground-speed", data["ground-speed"]);
    update("#data-gps-heading", data["heading-of-motion"]);
    update("#data-gps-satellites", data["num-sats"]);
    update("#data-gps-v-accuracy", data["vert-accuracy"]);
    update("#data-gps-h-accuracy", data["horz-accuracy"]);
    update("#data-gps-heading-accuracy", data["heading-accuracy"]);
}

function parseEnvironmentalData(data) {
    let p_data = data["pressure"];
    let t_data = data["temperature"];
    let h_data = data["humidity"];
    updateDataList("environmental-data-pressure", p_data);
    updateDataList("environmental-data-temperature", t_data);
    updateDataList("environmental-data-humidity", h_data);
}

/**
 * @brief Updates the data list with the new data, if the data is not in the
 * list it will be added.
 * @param {*} selector selector for the list
 * @param {*} list the json list of data
 */
function updateDataList(selector_id, data_list) {
    let list = document.querySelector("#" + selector_id);
    for (key in data_list) {
        let data_selector = selector_id + "-" + key;
        let data_p = document.querySelector("#" + data_selector);
        if (data_p == null) {
            let newP = document.createElement("p");
            newP.id = data_selector;
            newP.innerHTML = key + ": " + data_list[key]["unit"] + " " + data_list[key]["value"];
            list.appendChild(newP);
        } else {
            data_p.innerHTML = key + ": " + data_list[key]["unit"] + " " + data_list[key]["value"];
        }
    }
}

async function parseCalculatedData(data) {
    const selector = "calculated-data-content"
    let content_div = document.getElementById(selector);
    for (key in data) {
        if (content_div.querySelector("#" + selector + "-" + key) == null) {
            let newP = document.createElement("p");
            newP.id = selector + "-" + key;
            newP.innerHTML = key + ": " + data[key];
            content_div.appendChild(newP);
        } else {
            content_div.querySelector("#" + selector + "-" + key).innerHTML = key + ": " + data[key];
        }
    }
}

async function updateDataFrame() {
    if (connection_status) {
        let data_frame = await fetch('/api/get-data-frame')
        .then((response) => response.json())
        .then((data) => {return data})
        .catch((error) => {return false});

        if (data_frame.hasOwnProperty("0") == false) {
            console.log("Error fetching data frame");
            return;
        }

        let content_div = document.getElementById("data-frame-content");
        for (key in data_frame) {
            let selector = "data-frame-" + data_frame[key]["source"];
            if (content_div.querySelector("#" + selector) == null) {
                let newSourceDiv = document.createElement("div");
                newSourceDiv.id = selector;
                let newSourceHeader = document.createElement("h3");
                newSourceHeader.innerHTML = data_frame[key]["source"];
                newSourceDiv.appendChild(newSourceHeader);
                content_div.appendChild(newSourceDiv);
            }
            // Update the data
            let source_div = content_div.querySelector("#" + selector);
            let data_selector = selector + "-" + data_frame[key]["unit"];
            // Check if the data is in the list already
            let data_p = source_div.querySelector("#" + data_selector);
            if (data_p == null) {
                let newP = document.createElement("p");
                newP.id = data_selector;
                newP.innerHTML = data_frame[key]["unit"] + ": " + data_frame[key]["value"];
                source_div.appendChild(newP);
            } else {
                data_p.innerHTML = data_frame[key]["unit"] + ": " + data_frame[key]["value"];
            }

        }
        
    }
}

async function updateErrorFrame() {
    if (connection_status) {
        let error_frame = await fetch('/api/get-error-frame')
        .then((response) => response.json())
        .then((data) => {return data})
        .catch((error) => {return false});

        if (error_frame.hasOwnProperty("0") == false) {
            console.log("Error fetching error frame");
            return;
        }

        let content_div = document.getElementById("gfs-errors");
        for (key in error_frame) {
            let id = error_frame[key]["source"];
            let selector = "error-frame-" + id;
            if (content_div.querySelector("#" + selector) == null) {
                let newSourceDiv = document.createElement("div");
                newSourceDiv.id = selector;
                let newSourceHeader = document.createElement("h3");
                newSourceHeader.innerHTML = error_frame[key]["source"];
                newSourceDiv.appendChild(newSourceHeader);
                content_div.appendChild(newSourceDiv);
            }
            // Update the data
            let source_div = content_div.querySelector("#" + selector);
            let data_selector = selector + "-" + error_frame[key]["code"];
            // Check if the data is in the list already
            let data_p = source_div.querySelector("#" + data_selector);
            if (data_p == null) {
                let newP = document.createElement("p");
                newP.id = data_selector;
                newP.innerHTML = error_frame[key]["code"] + ": " + error_frame[key]["info"];
                source_div.appendChild(newP);
            } else {
                data_p.innerHTML = error_frame[key]["code"] + ": " + error_frame[key]["info"];
            }

        }
        
    }
}

async function updateExtensionStatuses() {
    if (connection_status) {
        let statuses = await fetch('/api/get-extension-statuses')
                        .then((response) => response.json())
                        .then((data) => {return data})
                        .catch((error) => {return false});

        for (key in statuses) {
            console.log("Updating " + key);
            let selector = "#ext_" + key;
            document.querySelector(selector).querySelector(".e_status").innerHTML = statuses[key];
        }

    }
}

function update(selector, value) {
    document.querySelector(selector).innerHTML = value;
}

async function runner() {
    try {
        await updateStatus();
        await updateGfsData();
        await updateDataFrame();
        await updateErrorFrame();
        await updateExtensionStatuses();
    } catch (error) {
        console.log("Error running runner.");
        console.log(error);
    }
    setTimeout(runner, 1000);
}

window.addEventListener("DOMContentLoaded", function() {
    updateConfig();
    runner();
});