const { DataTypes } = require("sequelize");

const sequelize = require("../sequelize_connection");

const LocationDataModel = sequelize.define(
  "LocationData",
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
    valid: {
      type: DataTypes.BOOLEAN,
      allowNull: false,
    },
    source: {
      // aprs-is, tcp, gdl, etc.
      type: DataTypes.STRING(20),
      allowNull: false,
    },
    latitude: {
      type: DataTypes.FLOAT,
    },
    longitude: {
      type: DataTypes.FLOAT,
    },
    altitude: {
      // altitude in meters
      type: DataTypes.FLOAT,
    },
    heading: {
      type: DataTypes.FLOAT,
    },
    horizontal_speed: {
      type: DataTypes.FLOAT,
    },
    vertical_speed: {
      type: DataTypes.FLOAT,
    },
    additional_data: {
      type: DataTypes.STRING,
    },
  },
  {
    updatedAt: false,
  }
);

module.exports = LocationDataModel;
