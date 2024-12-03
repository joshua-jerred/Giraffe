const { DataTypes } = require("sequelize");

module.exports = (sequelize) => {
  sequelize.define(
    "GroundStationLog",
    {
      id: {
        type: DataTypes.INTEGER,
        primaryKey: true,
        autoIncrement: true,
      },
      level: {
        type: DataTypes.STRING,
        allowNull: false,
      },
      message: {
        type: DataTypes.STRING,
        allowNull: false,
      },
    },
    {
      updatedAt: false,
    }
  );
};
