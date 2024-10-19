const sqlite3 = require("sqlite3").verbose();
const file_paths = require("../file_paths");

module.exports = class PostgresDatabase {
  constructor() {
    this.db = new sqlite3.Database(file_paths.GGS_SQLITE_DB, (error) => {
      if (error) {
        return console.error("Failed to create DB:", error.message);
      }
    });

    this.#createTables();
  }

  #createTables() {
    try {
      this.db.exec(
        `CREATE TABLE IF NOT EXISTS ggs_log (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            level              TEXT      NOT NULL,
            message            TEXT      NOT NULL,
            timestamp          INTEGER   NOT NULL
            );`
      );
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
        `CREATE TABLE IF NOT EXISTS GdlTelemetryData (
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
      // this.db.exec(`DROP TABLE GdlTelemetryData;`);
    } catch (err) {
      console.log(err);
    }
  }

  #getUnixTime() {
    return Math.round(+new Date() / 1000);
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
    var unix_time = Math.round(+new Date() / 1000);
    this.db.run(
      `INSERT INTO ggs_log (
            level,
            message,
            timestamp
        ) VALUES (
            $level,
            $message,
            $timestamp
        );`,
      {
        $level: level,
        $message: message,
        $timestamp: unix_time,
      }
    );
  }

  getRecentLogData(callback) {
    const LIMIT = 50;
    const query = `SELECT * FROM ggs_log ORDER BY timestamp DESC LIMIT ${LIMIT};`;
    this.db.all(query, [], (err, rows) => {
      if (err) {
        console.log(err);
        return;
      }
      callback(rows);
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

  addReceivedMessage(type, data, identifier) {
    let unix_time = Math.round(+new Date() / 1000);
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

  addReceivedTelemetryDataReport(data) {
    data.timestamp = this.#getUnixTime();

    this.db.run(
      `INSERT INTO GdlTelemetryData (
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
        $timestamp: data.timestamp,
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

  connect() {}
};
