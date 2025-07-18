/**
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 * @file   setting_sync.js
 * @brief  This file contains an interface to sync/update settings between the
 * GGS and GFS.
 * @todo This currently is just a copy of the data sync class. This needs to be
 * implemented.
 *
 * =*=======================*=
 * @author     Joshua Jerred https://joshuajer.red
 * @date       2023-06-11
 * @copyright  2023 (license to be defined)
 */

const DataMeta = require("../../../../project/metadata/gfs_configuration.json");
const { RequestMessage, parse } = require("giraffe-protocol");
const net = require("net");

const SETTING_NAME_EXCLUDED = ["SECTION_TYPE"];

/**
 * @brief This class handles syncing settings (not settings) between
 * GGS and GFS.
 */
module.exports = class GfsSettingSync {
  constructor(global_state) {
    this.global_state = global_state;
    this.resources = {};
    this.connected = false;
    this.#updateClassSettings();

    for (let category in DataMeta) {
      this.resources[category] = {
        meta: {
          last_request: new Date(),
          requests: 0,
        },
        data: {},
      };
      for (let data_item in DataMeta[category]) {
        if (SETTING_NAME_EXCLUDED.includes(category)) {
          continue;
        }
        this.resources[category].data[data_item] = "no data";
      }
    }
  }

  getConnectionStatus() {
    return this.connected;
  }

  /**
   * @brief This function returns the data for a given category.
   * @param {string} category The category to get the data for.
   * @returns {object} The data for the given category, or null if the
   * category does not exist.
   */
  getData(category) {
    if (!this.resources[category]) {
      return null;
    }
    return this.resources[category].data;
  }

  /**
   * @brief This function returns true if the given category exists.
   * @param {string} category The category to check.
   * @returns boolean True if the category exists, false otherwise.
   */
  doesDataExist(category) {
    return this.resources[category] ? true : false;
  }

  /**
   * @brief Called at a set interval at a higher level. This runs everything.
   */
  update() {
    this.#updateClassSettings();

    for (let category in this.resources) {
      if (
        this.resources[category].meta.last_request >
        new Date() - this.update_interval
      ) {
        continue;
      }
      this.#updateResource(category);
      this.resources[category].meta.last_request = new Date();
      this.resources[category].meta.requests++;
    }
  }

  /**
   * @brief Reads settings relevant to this class from the database and
   * stores them in the class.
   */
  #updateClassSettings() {
    this.update_interval = this.global_state.ggs_db.get(
      "settings",
      "ggs_settings",
      "gfs_config_interval"
    );

    this.address = this.global_state.ggs_db.get(
      "settings",
      "gfs_connection",
      "gfs_socket_address"
    );

    this.port = this.global_state.ggs_db.get(
      "settings",
      "gfs_connection",
      "gfs_socket_port"
    );
  }

  /**
   * @brief Used internally to set the local data of a given category.
   * Called by #updateResource.
   * @param {string} category The category to set the data for.
   * @param {Message} message The giraffe protocol message to get the data from.
   */
  #setLocalResource(category, message) {
    let data_section = message.bdy.dat;

    const SPECIAL_CASES = ["extensions", "adc_mappings"];
    if (SPECIAL_CASES.includes(category)) {
      this.resources[category].data = data_section;
      return;
    }

    for (let data_item in data_section) {
      try {
        this.resources[category].data[data_item] = data_section[data_item];
      } catch (e) {
        console.log(
          "Failed to set local resource for: " +
            data_item +
            " in " +
            data_section
        );
      }
    }
  }

  /**
   * Goes through the categories and requests the data from GFS.
   * @param {string} category The category to request the data for.
   */
  #updateResource(category) {
    let request = new RequestMessage("ggs", "gfs", "setting/" + category);
    let con = new net.Socket();
    let that = this;

    con.connect(this.port, this.address, function () {
      con.write(JSON.stringify(request));
    });

    con.on("data", function (data) {
      that.connected = true;
      try {
        let msg = parse(data.toString());
        if (msg.bdy.cde !== "ok") {
          console.log("Error: " + msg.bdy.dat);
          return;
        }
        that.#setLocalResource(category, msg);
      } catch (e) {
        console.log("Error getting settings for " + category);
      }
      con.destroy();
    });

    con.on("close", function () {});

    con.on("error", function (err) {
      that.connected = false;
    });
  }
};
