const { InfluxDB } = require("@influxdata/influxdb-client");

module.exports = class InfluxWriter {
  constructor(global_state) {
    this.status = "unknown";

    this.enabled = global_state.ggs_db.get(
      "settings",
      "influx_db",
      "influx_enabled"
    );

    if (!this.enabled) {
      this.status = "disabled";
      return;
    }

    this.token = global_state.ggs_db.get(
      "settings",
      "influx_db",
      "influx_db_token"
    );
    this.url =
      "http://" +
      global_state.ggs_db.get("settings", "influx_db", "influx_db_url");
    this.bucket = global_state.ggs_db.get(
      "settings",
      "influx_db",
      "influx_bucket"
    );
    this.org = global_state.ggs_db.get("settings", "influx_db", "influx_org");

    try {
      this.client = new InfluxDB({ url: this.url, token: this.token });
      this.writeClient = this.client.getWriteApi(this.org, this.bucket, "ns");
    } catch (error) {
      console.log(error);
    }
  }

  write(point, flush = false) {
    if (!this.enabled) {
      return;
    }

    try {
      this.writeClient.writePoint(point);
      if (flush) {
        this.writeClient.flush();
      }
    } catch (error) {
      console.log(error);
    }
  }

  flush() {
    if (!this.enabled) {
      return;
    }

    try {
      this.writeClient.flush();
    } catch (error) {
      console.log(error);
    }
  }
};

// let queryClient = client.getQueryApi(org);
// let fluxQuery = `from(bucket: "ggs")
//  |> range(start: -10m)
//  |> filter(fn: (r) => r._measurement == "measurement1")`;

// queryClient.queryRows(fluxQuery, {
//   next: (row, tableMeta) => {
//     const tableObject = tableMeta.toObject(row);
//     console.log(tableObject);
//   },
//   error: (error) => {
//     console.error("\nError", error);
//   },
//   complete: () => {
//     console.log("\nSuccess");
//   },
// });

// Execute a Flux Query
// let queryClient = client.getQueryApi(org);
// let fluxQuery = `from(bucket: "giraffe_ground_station")
//  |> range(start: -10m)
//  |> filter(fn: (r) => r._measurement == "measurement1")`;

// queryClient.queryRows(fluxQuery, {
//   next: (row, tableMeta) => {
//     const tableObject = tableMeta.toObject(row);
//     console.log(tableObject);
//   },
//   error: (error) => {
//     console.error("\nError", error);
//   },
//   complete: () => {
//     console.log("\nSuccess");
//   },
// });

// queryClient = client.getQueryApi(org);
// fluxQuery = `from(bucket: "giraffe_ground_station")
//  |> range(start: -10m)
//  |> filter(fn: (r) => r._measurement == "measurement1")
//  |> mean()`;

// queryClient.queryRows(fluxQuery, {
//   next: (row, tableMeta) => {
//     const tableObject = tableMeta.toObject(row);
//     console.log(tableObject);
//   },
//   error: (error) => {
//     console.error("\nError", error);
//   },
//   complete: () => {
//     console.log("\nSuccess");
//   },
// });
