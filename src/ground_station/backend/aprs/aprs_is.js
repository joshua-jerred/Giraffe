const net = require("net");
const version_info = require("../../../common/version");
const Timer = require("../helpers/timer");
const { Logger } = require("../helpers/logger");
const { processTNC2Data } = require("./aprs_tnc2_parse");

const Database = require("../db");

const log = new Logger("APRS-IS");

module.exports = class AprsIs {
  constructor(global_state) {
    this.global_state = global_state;
    this.connection_status = "unknown"; // "ok", "disabled", "error", "unknown", "critical-error"

    this.aprs_settings = {
      ground_station_callsign: "",
      flight_callsign: "",
    };
    this.aprs_is_settings = {
      enabled: false,
      address: "",
      port: 0,
      client_name: "",
      client_passcode: "",
      filters: "",
      log_other_packets: false,
    };

    this.#updateSettings();

    this.tcp_client = null;
    this.authenticated = false;

    this.setting_update_timer = new Timer(2500);
    this.setting_update_timer.restart();
    this.reconnect_timer = new Timer(1000);
    this.reconnect_timer.restart();
  }

  updateAndGetStatus() {
    if (this.setting_update_timer.isExpired()) {
      this.setting_update_timer.restart();
      this.#updateSettings();
    }

    if (this.connection_status === "critical-error") {
      return "critical-error";
    }
    if (!this.aprs_is_settings.enabled) {
      return "disabled";
    }

    if (this.tcp_client == null && this.reconnect_timer.isExpired()) {
      this.reconnect_timer.restart();
      this.#setupSocket();
    }

    return this.connection_status;
  }

  #setCrticalErrorState(error_message) {
    log.error("APRS-IS error:", error_message);
    this.connection_status = "critical-error";
    if (this.tcp_client) {
      this.tcp_client.destroy();
      this.tcp_client = null;
    }
  }

  #updateSettings() {
    this.aprs_settings = this.global_state.ggs_db.get("settings", "aprs");
    this.aprs_is_settings = this.global_state.ggs_db.get("settings", "aprs_is");
  }

  #setupSocket() {
    this.tcp_client = new net.Socket();
    // this.tcp_client.setTimeout(this.connection_timeout_s * 1000);
    this.tcp_client.connect(
      this.aprs_is_settings.port,
      this.aprs_is_settings.address,
      () => {
        log.debug("Connected to APRS-IS server, authenticating...");

        this.#authenticate();
      }
    );

    this.tcp_client.on("data", (data) => {
      let data_string = data.toString();

      if (data_string.startsWith("#")) {
        // This is a comment

        if (!this.authenticated) {
          if (data.toString().includes("verified")) {
            this.authenticated = true;
            log.info("Authenticated with APRS-IS server");
          } else if (data.toString().includes("unverified")) {
            this.setCrticalErrorState(
              "Failed to authenticate to APRS-IS server: " +
                data.toString().trim()
            );
          }
        }

        return;
      }

      if (!this.authenticated) {
        return;
      }

      this.connection_status = "ok";
      this.#processPacket(data_string);
      // Handle incoming data here
    });

    this.tcp_client.on("error", (error) => {
      log.error("Error connecting to APRS-IS server:", error);
      this.connection_status = "error";
    });

    this.tcp_client.on("timeout", () => {
      log.error("Connection to APRS-IS server timed out");
      this.connection_status = "error";
      this.tcp_client.destroy();
    });

    this.tcp_client.on("close", () => {
      log.debug("Connection to APRS-IS server closed");
      this.connection_status = "unknown";
      this.tcp_client = null;
    });
  }

  #authenticate() {
    // @todo Filters!
    const auth_str = `user ${this.aprs_is_settings.client_name} pass ${this.aprs_is_settings.client_passcode} vers giraffe-ground-station ${version_info.version_string}\r`;
    this.tcp_client.write(auth_str, (err) => {
      if (err) {
        this.#setCrticalErrorState(
          "Error sending authentication string:" + err
        );
        return false;
      }
    });
  }

  #processPacket(data) {
    const packet_data = processTNC2Data(data);
    if (!packet_data) {
      log.error("Failed to parse APRS-IS data:", data);
      return;
    }
    // console.log("Packet data:", packet_data);

    const packet_origin =
      packet_data.source_callsign === this.aprs_settings.ground_station_callsign
        ? "ground_station"
        : packet_data.source_callsign === this.aprs_settings.flight_callsign
        ? "flight"
        : "other";

    if (packet_origin === "other" && !this.aprs_is_settings.log_other_packets) {
      log.debug("Ignoring packet from unknown source:", source_callsign);
      return;
    }

    // Log the raw packet data
    Database.models.AprsPacket.create({
      origin: packet_origin,
      source_address: packet_data.source_callsign,
      destination_address: packet_data.destination_callsign,
      path: packet_data.digi_path,
      data: packet_data.information_field,
    });

    if (
      packet_origin === "flight" &&
      packet_data.parsed &&
      packet_data.parsed.position
    ) {
      const position = packet_data.parsed.position;
      const location = {
        valid: true,
        source: "aprs",
        timestamp: packet_data.timestamp,
        latitude: position.latitude,
        longitude: position.longitude,
        altitude: position.altitude,
        horizontal_speed: position.speed,
        heading: position.course,
        additional_data: packet_data.information_field,
      };

      this.global_state.flight_data.location_data.addNewLocationReport(
        "aprs-is",
        location
      );
    }

    // if (parsed) {
    //   // Log the parsed data
    //   log.debug("Parsed APRS-IS data:", parsed_data);

    //   // Update the global state with the parsed data
    //   // this.global_state.
    // } else {
    //   log.warning("Failed to parse APRS-IS data:", information_field);
    // }
  }
};
