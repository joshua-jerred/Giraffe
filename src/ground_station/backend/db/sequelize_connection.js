const Sequelize = require("sequelize");
const ggs_db = require("./ggs_db");

// const log = require("../log");

const LOG_QUERY_TO_CONSOLE = false;
// const MASK_COMMON_LOG_ITEMS = true;
// const COMMON_LOG_CONTENTS = [
//   // Below are substrings for identifying queries that should be masked from the log
//   "SELECT 1+1 AS result",
//   "SELECT i.relname AS name, ix.indisprimary AS primary, ix.indisunique AS unique, ix.indkey AS indkey, array_agg(a.attnum) as column_indexes",
//   "SELECT table_name FROM information_schema.tables WHERE table_schema = 'public' AND table_name = 'Users'",
//   "Executing (default): SELECT table_name FROM information_schema.tables WHERE table_schema = 'public' AND table_name = 'AuthTokens'",
// ];

const settigns = new ggs_db();
const db_to_use = settigns.get(
  "settings",
  "ggs_settings",
  "ggs_database_selection"
);
console.log("Using database:", db_to_use);
const mysql_config = settigns.get("settings", "mysql");

const sequelize = new Sequelize(
  mysql_config.database,
  mysql_config.username,
  mysql_config.password,
  {
    host: mysql_config.host,
    port: mysql_config.port,
    dialect: "mysql",
    connectionTimeoutMillis: 3000,

    // dialect: "postgres",
    // host: env.PGHOST,
    // port: env.PGPORT,
    // database: env.PGDATABASE,
    // username: env.PGUSER,
    // password: env.PGPASSWORD,
    logging: (msg) => {
      if (LOG_QUERY_TO_CONSOLE) {
        // Mask away DB authentication logging as it's called periodically.
        // if (MASK_COMMON_LOG_ITEMS) {
        // for (const content of COMMON_LOG_CONTENTS) {
        //   if (msg.includes(content)) {
        //     return;
        //   }
        // }
        // }

        // log.info("DB Query:", msg);
        console.log("DB Query:", msg);
      }
    },
  }
  // ssl: true,
  // clientMinMessages: 'notice',
);

module.exports = sequelize;
