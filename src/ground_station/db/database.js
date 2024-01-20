const sqlite3 = require("sqlite3").verbose();
const db_file = "./db/ggs.sqlite3";

module.exports = class PostgresDatabase {
  constructor() {
    this.db = new sqlite3.Database(db_file, (error) => {
      if (error) {
        return console.error("Failed to create DB:", error.message);
      }
    });

    this.createTables();
  }

  createTables() {
    try {
      this.db.exec(
        `CREATE TABLE IF NOT EXISTS ggs_log (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            level              TEXT      NOT NULL,
            message            TEXT      NOT NULL,
            timestamp          INTEGER   NOT NULL
            );`
      );
    } catch (err) {
      console.log(err);
    }
    try {
      this.db.exec(
        `CREATE TABLE IF NOT EXISTS aprs_fi (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name               TEXT      NOT NULL,
            time               INTEGER   NOT NULL UNIQUE,
            lasttime           INTEGER   NOT NULL,
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
    } catch (err) {
      console.log(err);
    }
  }

  addAprsFiData(data) {
    // check if this timestamp already exists
    // if so, return
    this.db.get(
      `SELECT 1 as e FROM aprs_fi WHERE time = $time;`,
      {
        $time: data.time,
      },
      (err, rows) => {
        if (err) {
          console.log(err);
          return;
        }
        if (rows && rows.e == 1) {
          return;
        }

        this.#insertAprsFiData(data);
      }
    );
  }

  getRecentAprsFiData(callback) {
    const LIMIT = 20;
    const query = `SELECT * FROM aprs_fi ORDER BY time DESC LIMIT ${LIMIT};`;
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
    const LIMIT = 20;
    const query = `SELECT * FROM ggs_log ORDER BY timestamp DESC LIMIT ${LIMIT};`;
    this.db.all(query, [], (err, rows) => {
      if (err) {
        console.log(err);
        return;
      }
      callback(rows);
    });
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
