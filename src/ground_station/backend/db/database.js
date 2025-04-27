const file_paths = require("../file_paths");

const sqlite3 = require("sqlite3").verbose();
const { Sequelize } = require("sequelize");

module.exports = class GroundStationDatabase {
  constructor(global_state) {
    this.connected = false;
    this.sequelize = null;

    this.db = new sqlite3.Database(file_paths.GGS_SQLITE_DB, (error) => {
      if (error) {
        return console.error("Failed to create DB:", error.message);
      }
    });

    const db_to_use = global_state.ggs_db.get(
      "settings",
      "ggs_settings",
      "ggs_database_selection"
    );
    console.log("Using database:", db_to_use);

    if (db_to_use === "sqlite") {
      console.log("SQLite not implemented");
    } else if (db_to_use === "mysql") {
      const mysql_config = global_state.ggs_db.get("settings", "mysql");
      this.sequelize = new Sequelize(
        mysql_config.database,
        mysql_config.username,
        mysql_config.password,
        {
          host: mysql_config.host,
          port: mysql_config.port,
          dialect: "mysql",
          logging: mysql_config.log_sql_to_console
            ? (msg) => this.#sequelizeLog(msg)
            : false,
        }
      );

      this.#defineSequelizeModels();

      this.sequelize
        .authenticate()
        .then(() => {
          this.connected = true;
          this.sequelize.sync();
        })
        .catch((err) => {
          console.error("Failed to connect to MySQL database:", err);
        });

      console.log(this.sequelize.models);
    } else {
      console.error("Invalid database selection");
    }

    this.#createTables();
  }

  #sequelizeLog(message) {
    console.log("DB Query:", message);
  }

  #defineSequelizeModels() {
    const models = [
      require("./models/GroundStationLog.model"),
      // require("./models/GdlServerLog.model"),
      // require("./models/GdlReceivedMessages.model"),
      // require("./models/GdlSentMessages.model"),
      // require("./models/GdlReceivedLocMessages.model"),
      // require("./models/AprsFi.model"),
      // require("./models/ReceivedAprsTelemetryData.model"),
    ];

    for (const model of models) {
      model(this.sequelize);
    }
  }

  #createTables() {
    try {
      this.db.exec(`CREATE TABLE IF NOT EXISTS GroundStationLog (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            level              TEXT      NOT NULL,
            message            TEXT      NOT NULL,
            timestamp          INTEGER   NOT NULL
            );`);
      this.db.exec(
        `CREATE TABLE IF NOT EXISTS gdl_server_log (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            level              TEXT      NOT NULL,
            message            TEXT      NOT NULL,
            timestamp          INTEGER   NOT NULL
            );`
      );
      this.db.exec(
        `CREATE TABLE IF NOT EXISTS gdl_received_messages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            type               TEXT      NOT NULL,
            data               TEXT      NOT NULL,
            identifier         TEXT      NOT NULL,
            timestamp          INTEGER   NOT NULL
            );`
      );
      this.db.exec(
        `CREATE TABLE IF NOT EXISTS gdl_sent_messages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            type               TEXT      NOT NULL,
            data               TEXT      NOT NULL,
            identifier         TEXT      NOT NULL,
            timestamp          INTEGER   NOT NULL
            );`
      );
      this.db.exec(
        `CREATE TABLE IF NOT EXISTS gdl_received_loc_messages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            latitude               REAL      NOT NULL,
            longitude              REAL      NOT NULL,
            altitude               REAL      NOT NULL,
            speed                  REAL      NOT NULL,
            heading                REAL      NOT NULL,
            time_code              TEXT      NOT NULL
            );`
      );
      this.db.exec(
        `CREATE TABLE IF NOT EXISTS aprs_fi (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name               TEXT      NOT NULL,
            time               INTEGER   NOT NULL,
            lasttime           INTEGER   NOT NULL UNIQUE,
            lat                REAL      NOT NULL,
            lng                REAL      NOT NULL,
            altitude           REAL      NOT NULL,
            speed              REAL      DEFAULT -1,
            course             INTEGER   DEFAULT -1,
            symbol             TEXT      DEFAULT '',
            srccall            TEXT      DEFAULT '',
            dstcall            TEXT      DEFAULT '',
            comment            TEXT      DEFAULT '',
            path               TEXT      DEFAULT '',
            phg                TEXT      DEFAULT '',
            status             TEXT      DEFAULT '',
            status_lasttime   INTEGER   DEFAULT -1
            );`
      );
      this.db.exec(
        `CREATE TABLE IF NOT EXISTS ReceivedAprsTelemetryData (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp              INTEGER NOT NULL,
            sequence_number        INTEGER NOT NULL,
            a1                     INTEGER NOT NULL,
            a2                     INTEGER NOT NULL,
            a3                     INTEGER NOT NULL,
            a4                     INTEGER NOT NULL,
            a5                     INTEGER NOT NULL,
            d1                     INTEGER NOT NULL,
            d2                     INTEGER NOT NULL,
            d3                     INTEGER NOT NULL,
            d4                     INTEGER NOT NULL,
            d5                     INTEGER NOT NULL,
            d6                     INTEGER NOT NULL,
            d7                     INTEGER NOT NULL,
            d8                     INTEGER NOT NULL,
            comment                TEXT    NOT NULL
            );`
      );
    } catch (err) {
      console.log(err);
    }
  }

  #getUnixTime() {
    return Math.round(new Date().getTime() / 1000);
  }

  addAprsFiData(data) {
    // check if this timestamp already exists
    // if so, return
    this.db.get(
      `SELECT 1 as e FROM aprs_fi WHERE lasttime = $lasttime;`,
      {
        $lasttime: data.lasttime,
      },
      (err, rows) => {
        if (err) {
          console.log(err);
          return;
        }
        if (rows && rows.e == 1) {
          console.log("already exists");
          return;
        }
        console.log("inserting");
        this.#insertAprsFiData(data);
      }
    );
  }

  getRecentAprsFiData(callback) {
    const LIMIT = 20;
    const query = `SELECT * FROM aprs_fi ORDER BY lasttime DESC LIMIT ${LIMIT};`;
    this.db.all(query, [], (err, rows) => {
      if (err) {
        console.log(err);
        return;
      }
      callback(rows);
    });
  }

  addLog(level, message) {
    if (!this.connected) {
      /// We are not connected yet, try again in 100ms
      setTimeout(() => {
        this.addLog(level, message);
      }, 100);
      return;
    }

    this.sequelize.models.GroundStationLog.create({
      level: level,
      message: message,
    });
  }

  getRecentLogData(callback) {
    // broken
    // const LIMIT = 50;
    // this.sequelize.models.GroundStationLog.findAll({
    //   order: [["id", "DESC"]],
    //   limit: LIMIT,
    // }).then((rows) => {
    //   callback(rows);
    // });
  }

  /**
   * @param {number} id - The id of the log entry to delete
   * @param {function(err, updates)} callback - Callback function to call after
   * the delete operation is complete. err will be either null or contain an
   * error message, updates will be -1 on failure, or the number of rows updated
   * on success.
   */
  deleteLogEntry(id, callback) {
    // this.db.run(`DELETE FROM GroundStationLog WHERE id=?`, id, function (err) {
    //   if (err) {
    //     callback(err, -1);
    //     return;
    //   }

    //   callback(null, this.changes);
    // });

    this.sequelize.models.GroundStationLog.destroy({
      where: {
        id: id,
      },
    })
      .then((updates) => {
        callback(null, updates);
      })
      .catch((err) => {
        console.log(err);
        callback(err, -1);
      });
  }

  deleteAllLogEntries(callback) {
    this.sequelize.models.GroundStationLog.truncate()
      .then(() => {
        callback(null, 0);
      })
      .catch((err) => {
        console.log(err);
        callback(err, -1);
      });
  }

  addGdlServerLogItem(item) {
    this.db.run(
      `INSERT INTO gdl_server_log (
            level,
            message,
            timestamp
        ) VALUES (
            $level,
            $message,
            $timestamp
        );`,
      {
        $level: item.level,
        $message: item.message,
        $timestamp: item.timestamp,
      }
    );
  }

  addReceivedMessage(type, data, identifier, unix_time) {
    this.db.run(
      `INSERT INTO gdl_received_messages (
            type,
            data,
            identifier,
            timestamp
        ) VALUES (
            $type,
            $data,
            $identifier,
            $timestamp
        );`,
      {
        $type: type,
        $data: data,
        $identifier: identifier,
        $timestamp: unix_time,
      }
    );
  }

  getReceivedMessages(callback) {
    const LIMIT = 50;
    const query = `SELECT * FROM gdl_received_messages ORDER BY timestamp DESC LIMIT ${LIMIT};`;
    this.db.all(query, [], (err, rows) => {
      if (err) {
        console.log(err);
        return;
      }
      callback(rows);
    });
  }

  addSentMessage(type, data, identifier) {
    const unix_time = Math.round(+new Date() / 1000);
    this.db.run(
      `INSERT INTO gdl_sent_messages (
            type,
            data,
            identifier,
            timestamp
        ) VALUES (
            $type,
            $data,
            $identifier,
            $timestamp
        );`,
      {
        $type: type,
        $data: data,
        $identifier: identifier,
        $timestamp: unix_time,
      }
    );
  }

  getSentMessages(callback) {
    const LIMIT = 50;
    const query = `SELECT * FROM gdl_sent_messages ORDER BY timestamp DESC LIMIT ${LIMIT};`;
    this.db.all(query, [], (err, rows) => {
      if (err) {
        console.log(err);
        return;
      }
      callback(rows);
    });
  }

  addReceivedLocation(
    latitude,
    longitude,
    altitude,
    speed,
    heading,
    time_code
  ) {
    this.db.run(
      `INSERT INTO gdl_received_loc_messages (
            latitude,
            longitude,
            altitude,
            speed,
            heading,
            time_code
        ) VALUES (
            $latitude,
            $longitude,
            $altitude,
            $speed,
            $heading,
            $time_code
        );`,
      {
        $latitude: latitude,
        $longitude: longitude,
        $altitude: altitude,
        $speed: speed,
        $heading: heading,
        $time_code: time_code,
      }
    );
  }

  #insertAprsFiData(data) {
    if (
      !data.name ||
      !data.time ||
      !data.lasttime ||
      !data.lat ||
      !data.lng ||
      !data.altitude
    ) {
      console.log("Missing required data");
      return;
    }

    if (!data.speed) {
      data.speed = -1;
    }
    if (!data.course) {
      data.course = -1;
    }
    if (!data.symbol) {
      data.symbol = "";
    }
    if (!data.srccall) {
      data.srccall = "";
    }
    if (!data.dstcall) {
      data.dstcall = "";
    }
    if (!data.comment) {
      data.comment = "";
    }
    if (!data.path) {
      data.path = "";
    }
    if (!data.phg) {
      data.phg = "";
    }
    if (!data.status) {
      data.status = "";
    }
    if (!data.status_lasttime) {
      data.status_lasttime = -1;
    }

    try {
      this.db.run(
        `INSERT INTO aprs_fi (
            name,
            time,
            lasttime,
            lat,
            lng,
            altitude,
            speed,
            course,
            symbol,
            srccall,
            dstcall,
            comment,
            path,
            phg,
            status,
            status_lasttime
        ) VALUES (
            $name,
            $time,
            $lasttime,
            $lat,
            $lng,
            $altitude,
            $speed,
            $course,
            $symbol,
            $srccall,
            $dstcall,
            $comment,
            $path,
            $phg,
            $status,
            $status_lasttime
        );`,
        {
          $name: data.name,
          $time: data.time,
          $lasttime: data.lasttime,
          $lat: data.lat,
          $lng: data.lng,
          $altitude: data.altitude,
          $speed: data.speed,
          $course: data.course,
          $symbol: data.symbol,
          $srccall: data.srccall,
          $dstcall: data.dstcall,
          $comment: data.comment,
          $path: data.path,
          $phg: data.phg,
          $status: data.status,
          $status_lasttime: data.status_lasttime,
        }
      );
    } catch (err) {
      console.log(err);
    }
  }

  /// @deprecated
  connect() {}

  /**
   * Add telemetry data to the database
   * @param {object} data - The telemetry data to add to the database
   * @param {number} unix_time - Unix timestamp of when the data was received
   */
  addReceivedTelemetryDataReport(data, unix_time) {
    this.db.run(
      `INSERT INTO ReceivedAprsTelemetryData (
          timestamp,
          sequence_number,
          a1, a2, a3, a4, a5,
          d1, d2, d3, d4, d5, d6, d7, d8,
          comment
      ) VALUES (
          $timestamp,
          $sequence_number,
          $a1, $a2, $a3, $a4, $a5,
          $d1, $d2, $d3, $d4, $d5, $d6, $d7, $d8,
          $comment
        );`,
      {
        $timestamp: unix_time,
        $sequence_number: data.sequence_number,
        $a1: data.a1,
        $a2: data.a2,
        $a3: data.a3,
        $a4: data.a4,
        $a5: data.a5,
        $d1: data.d1,
        $d2: data.d2,
        $d3: data.d3,
        $d4: data.d4,
        $d5: data.d5,
        $d6: data.d6,
        $d7: data.d7,
        $d8: data.d8,
        $comment: data.comment,
      }
    );
  }

  async getAprsTelemetryData(start_time, end_time, limit, callback) {
    const MAX_LIMIT = 150;
    const LIMIT = limit > MAX_LIMIT ? MAX_LIMIT : limit;
    const query1 = `SELECT * FROM ReceivedAprsTelemetryData
    WHERE timestamp >= ${start_time} AND timestamp <= ${end_time}
    ORDER BY timestamp DESC
    LIMIT ${LIMIT}`;

    this.db.all(query1, [], (err, rows) => {
      if (err) {
        console.log(err);
        return;
      }
      callback(rows);
    });
  }

  async getNumAprsTelemetryPackets() {
    return new Promise((resolve, reject) => {
      this.db.get(
        `SELECT COUNT(*) as count FROM ReceivedAprsTelemetryData;`,
        [],
        (err, row) => {
          if (err) {
            reject(err);
          }
          resolve(row.count);
        }
      );
    });
  }

  async getLatestAprsTelemetryPacket() {
    return new Promise((resolve, reject) => {
      this.db.get(
        `SELECT * FROM ReceivedAprsTelemetryData ORDER BY timestamp DESC LIMIT 1;`,
        [],
        (err, row) => {
          if (err) {
            reject(err);
          }
          resolve(row);
        }
      );
    });
  }

  deleteAprsTelemetryDataEntry(id, callback) {
    this.db.run(
      `DELETE FROM ReceivedAprsTelemetryData WHERE id=?`,
      id,
      function (err) {
        if (err) {
          callback(err, -1);
          return;
        }

        callback(null, this.changes);
      }
    );
  }

  deleteAllAprsTelemetryDataEntries(callback) {
    this.db.run(`DELETE FROM ReceivedAprsTelemetryData`, function (err) {
      if (err) {
        callback(err, -1);
        return;
      }

      callback(null, this.changes);
    });
  }

  addFakeAprsTelemetryData(num = 1) {
    const start_time = this.#getUnixTime() - 3600;
    let current = 5;
    let up = true;
    for (let i = 0; i <= num; i++) {
      const random = Math.floor(Math.random() * 10);
      current += up ? random : -random;
      if (current > 230) {
        up = false;
      } else if (current < 0) {
        up = true;
        current = 5;
      }

      const dummy_data = {
        sequence_number: i,
        a1: current,
        a2: current * 0.5,
        a3: 70,
        a4: 200,
        a5: 10,
        d1: 0,
        d2: 1,
        d3: 0,
        d4: 0,
        d5: 0,
        d6: 0,
        d7: 1,
        d8: 0,
        comment: "Test Data",
      };

      let unix_time = start_time + i * 60;
      this.addReceivedTelemetryDataReport(dummy_data, unix_time);
    }
  }
};
