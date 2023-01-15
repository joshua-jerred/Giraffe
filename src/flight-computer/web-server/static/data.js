var config_loaded = false;

async function updateConfig() {
    let connected = await getConnectionStatus();
    console.log("Connected: " + connected);
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
            let update_interval = ext["update-interval"];
            let flight_critical = ext["flight-critical"];
            let extra_info = "";

            if (interface == "i2c") {
                if (ext.hasOwnProperty("i2c-bus")) {
                    extra_info += "I2C(" + ext["i2c-bus"] + ", ";
                } else {
                    extra_info += "I2C(ERR, "
                }
                if (ext.hasOwnProperty("i2c-address")) {
                    extra_info += ext["i2c-address"] + ")";
                } else {
                    extra_info += "ERR)";
                }
            } else if (interface == "onewire") {
                if (ext.hasOwnProperty("onewire-id")) {
                    extra_info += "1WID(" + ext["onewire-id"] + ")";
                } else {
                    extra_info += "1WID(ERR)";
                }
            } else if (interface == "spi") {
                if (ext.hasOwnProperty("spi-bus")) {
                    extra_info += "SPI(" + ext["spi-bus"] + ")";
                } else {
                    extra_info += "SPI(ERR)";
                }
            } else if (interface == "uart") {
                if (ext.hasOwnProperty("uart-bus")) {
                    extra_info += "UART(" + ext["uart-bus"] + ")";
                } else {
                    extra_info += "UART(ERR)";
                }
            } else if (interface == "gpio") {
                if (ext.hasOwnProperty("gpio-pins")) {
                    extra_info += "GPIO(" + ext["gpio-pins"] + ")";
                } else {
                    extra_info += "GPIO(ERR)";
                }
            } else {
                extra_info = "N/A";
            }
            let new_extension = document.querySelector("#ext_template").cloneNode(true);
            new_extension.style.display = "table-row";
            new_extension.id = "ext_" + id;
            new_extension.querySelector(".e_id").innerHTML = id;
            new_extension.querySelector(".e_name").innerHTML = name;
            new_extension.querySelector(".e_type").innerHTML = type;
            new_extension.querySelector(".e_category").innerHTML = category;
            new_extension.querySelector(".e_interface").innerHTML = interface;
            new_extension.querySelector(".e_interval").innerHTML = update_interval;
            new_extension.querySelector(".e_flight-critical").innerHTML = flight_critical;
            new_extension.querySelector(".e_extra-info").innerHTML = extra_info;
            new_extension.querySelector(".e_status").innerHTML = "Unknown";
            console.log(new_extension);
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

window.addEventListener("DOMContentLoaded", function() {
    updateConfig();
});