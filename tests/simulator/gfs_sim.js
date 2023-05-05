var net = require("net");
const cfg = require("./sim_params.json").gfs;
const parse = require("giraffe-protocol").parse;
const Message = require("giraffe-protocol").Message;
const DataResponse = require("giraffe-protocol").DataResponse;

let state = {
  config: {
    general: {
      project_name: "GFS Simulator",
      main_board_type: "pi_zero_w_2",
      starting_procedure: "testing",
    },
    debug: {
      debug_mode_print_errors: true,
      console_enabled: true,
      console_update_interval: 1000,
      web_socket_enabled: true,
      web_socket_port: 7893,
    },
    telemetry: {
      general: {
        telemetry_enabled: true,
        call_sign: "GFS",
      },
      aprs: {
        enabled: true,
        telemetry_packets: true,
        position_packets: true,
        frequency: 144.39,
        ssid: 0,
        destination_address: "APRS",
        destination_ssid: 0,
        symbol_table: "/",
        telemetry_destination: "APRS",
        comment: "GFS Simulator",
      },
      sstv: {
        enabled: true,
        frequency: 145.8,
        mode: "robot36",
        overlay_data: true,
      },
      data_packets: {
        enabled: true,
        frequency: 144.39,
        mode: "bpsk500",
        morse_callsign: true,
        comment: "GFS Simulator",
      },
    },
  },
  status: {
    system: {
      time_utc: "2020-01-01T00:00:00Z",
      date: "2020-01-01",
      uptime: 10,
      ldavg: [0.1, 0.2, 0.3],
      cpu_temp: 40,
      ram_usage: 0.1,
      disk_usage: 0.1,
      bat_voltage: 12.5,
    },
    general: {
      gfs_uptime: 10,
      health: "good",
      num_errors: 0,
    },
    modules: {
      config: "running",
      telemetry: "running",
      data: "running",
      console: "running",
      network: "running",
    },
    extensions: {
      "temp-1": "running",
      "gps-2": "running",
    },
    data_stream: {
      total_data_packets: 11,
      current_data_packets: 2,
      total_gps_packets: 12,
      current_gps_packets: 2,
      total_error_packets: 13,
      current_error_packets: 4,
    },
    telemetry: {
      tx_log: {
        size: 5,
        max_size: 10,
      },
      tx_queue: {
        size: 5,
        max_size: 10,
      },
      radios: {
        "radio-1": "good"
      }
    },
    data_log: {
      enabled: true,
      file_name: "data_log.csv",
      file_size: 10,
    },
  },
  data: {
    critical: {
      flight_phase: "ascent",
      gps_valid: true,
      gps: {
        lat: 0,
        lon: 0,
        alt: 0,
        fix: "3D",
      },
      pressure_valid: true,
      pressure: 1013.25,
      battery_valid: true,
      battery_voltage: 12.5,
      system_valid: true,
      system: {
        mem_usage: 0.1,
        disk_usage: 0.1,
      },
      radio_status: "good",
    },
    position: {
      gps: {
        source: "gps-1",
        time: "2020-01-01T00:00:00Z",
        fix: "3D",
        sats: 10,
        lat: 0.001,
        lon: 0.001,
        alt: 1000,
        horizontal_speed: 10,
        vertical_speed: 10,
        heading: 0,
        accuracy: {
          horizontal: 10,
          vertical: 10,
          heading: 10,
        },
      },
      imu: {
        source: "imu-1",
        time: "2020-01-01T00:00:00Z",
        accel: {
          x: 0,
          y: 0,
          z: 0,
        },
        gyro: {
          x: 0,
          y: 0,
          z: 0,
        },
      }
    },
  },
  errors: {
    "error:1":"msg"
  },
  log_files: {
    
  }
};
function getItem(resource, category, subcategory, env) {
  if (!(resource in state)) {
    return null;
  }
  if (!(category in state[resource])) {
    return null;
  }

  if (subcategory in state[resource][category]) {
    return state[resource][category][subcategory];
  } else if (subcategory === "all") {
    return state[resource][category];
  } else {
    return null;
  }
}

function dataResponse(dataRequest, env) {
  let src = dataRequest.src;
  let id = dataRequest.id;
  let params = dataRequest.body.params;
  let resource = params.resource;
  let category = params.category;
  let subcategory = params.subcategory;
  if (!params || !resource || !category || !subcategory) {
    return new Message("gfs", src, "info", "error", id, {
      error: "Invalid parameters",
    });
  }

  let item = getItem(resource, category, subcategory, env);
  if (item === null) {
    return new Message("gfs", src, "info", "error", id, {
      error: "Item not found",
    });
  } else {
    return new DataResponse("gfs", src, id, item);
  }
}

module.exports = (env) => {
  var server = net.createServer(function (s) {
    console.log(`Connected to: ${s.remoteAddress}:${s.remotePort}`);
    s.on("data", function (data) {
      //console.log("Received: " + typeof data);
      let msg = parse(data.toString());
      if (msg.typ === "req" && msg.cat === "data") {
        let rsp = dataResponse(msg, env);
        s.write(JSON.stringify(rsp));
        return;
      }
      s.write(JSON.stringify(state));
    });

    s.on("close", function (data) {
      //console.log(`Closed: ${s.remoteAddress}:${s.remotePort}`);
    });

    s.on("error", function (err) {
      console.log(err);
    });
  });

  server.listen(cfg.port, cfg.host);
};
