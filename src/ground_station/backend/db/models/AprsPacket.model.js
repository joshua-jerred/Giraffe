const { DataTypes } = require("sequelize");

const sequelize = require("../sequelize_connection");

const AprsPacketModel = sequelize.define(
  "AprsPacket",
  {
    id: {
      type: DataTypes.INTEGER,
      primaryKey: true,
      autoIncrement: true,
    },
    timestamp: {
      type: DataTypes.DATE,
      allowNull: false,
      defaultValue: DataTypes.NOW,
    },
    origin: {
      // flight, ground_station, or other
      type: DataTypes.STRING(20),
      allowNull: false,
    },
    source_address: {
      type: DataTypes.STRING(20),
      allowNull: false,
    },
    destination_address: {
      type: DataTypes.STRING(20),
      allowNull: false,
    },
    path: {
      type: DataTypes.STRING(100),
      allowNull: true,
    },
    data: {
      type: DataTypes.TEXT,
      allowNull: false,
    },
  },
  {
    updatedAt: false,
  }
);

module.exports = AprsPacketModel;
