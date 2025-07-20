const { DataTypes } = require("sequelize");

const sequelize = require("../sequelize_connection");

const SquencerStepModel = sequelize.define(
  "SquencerStepModel",
  {
    id: {
      type: DataTypes.INTEGER,
      primaryKey: true,
      autoIncrement: true,
    },
    sequencer_run_id: {
      type: DataTypes.INTEGER,
      allowNull: false,
    },
    timestamp: {
      type: DataTypes.DATE,
      allowNull: false,
      defaultValue: DataTypes.NOW,
    },
    step_label: {
      type: DataTypes.STRING(100),
      allowNull: false,
    },
    status: {
      type: DataTypes.STRING(20),
      allowNull: false,
      defaultValue: "not started", // not started, in progress, completed, failed
    },
  },
  {
    updatedAt: false,
  }
);

module.exports = SquencerStepModel;
