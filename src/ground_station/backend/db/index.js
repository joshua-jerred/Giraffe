const Logger = require("../helpers/logger");
const sequelize = require("./sequelize_connection");

// Load models
require("./models/AprsPacket.model");
require("./models/GroundStationLog.model");
require("./models/LocationData.model");
require("./models/SequencerStep.model");

sequelize.sync().then(() => {
  Logger.debug("Database", "Database synced successfully");
});

module.exports = {
  async checkConnectionStatus() {
    try {
      await sequelize.authenticate();
      return new Promise((resolve, reject) => {
        resolve(["connected", null]);
      });
    } catch (error) {
      return new Promise((resolve, reject) => {
        resolve(["error", error.toString()]);
      });
    }
  },

  models: {
    AprsPacket: require("./models/AprsPacket.model"),
    GroundStationLog: require("./models/GroundStationLog.model")(sequelize),
    LocationData: require("./models/LocationData.model"),
    SequencerStep: require("./models/SequencerStep.model"),
  },
  sequelize: sequelize,
};
