const version_info = require("../../common/version");
const superagent = require("superagent");

const USER_AGENT =
  `Giraffe/${version_info.version}-${version_info.stage} ` +
  `(Giraffe Ground Station, GGS; +${version_info.git_repo}; ` +
  `${version_info.git_hash}) Node.js/${process.version}`;

APRS_FI_URL = "https://api.aprs.fi/api/get";
// APRS_FI_URL = "http://localhost:8080/api/get";

module.exports = class AprsFi {
  constructor(global_state) {
    this.global_state = global_state;
    this.most_recent_packet = {};
    this.enabled = false;

    this.time_of_last_get_attempt = new Date();
    this.connection_status = "unknown";
    this.failed = 0; // count to 2, then disable.

    this.#updateSettings();
  }

  updateAndGetStatus() {
    this.#updateSettings();
    if (!this.enabled) {
      return "disabled";
    }

    if (this.failed >= 2) {
      this.global_state.error(
        "aprs_fi failed to connect 2 times in a row. Disabling."
      );
      this.global_state.ggs_db.set("settings", "aprs_fi", "enabled", false);
      this.failed = 0;
      return "error";
    }

    let seconds_elapsed = (new Date() - this.time_of_last_get_attempt) / 1000;

    // console.log(seconds_elapsed);
    if (seconds_elapsed > this.check_interval_s) {
      console.log("requesting aprs.fi data");
      this.time_of_last_get_attempt = new Date();
      this.#sendApiRequest();
    }

    return this.connection_status;
  }

  forceDisable() {
    this.enabled = false;
  }

  #updateSettings() {
    this.enabled = this.global_state.ggs_db.get(
      "settings",
      "aprs_fi",
      "enabled"
    );
    this.name = this.global_state.ggs_db.get("settings", "aprs_fi", "name");
    this.apikey = this.global_state.ggs_db.get(
      "settings",
      "aprs_fi",
      "api_key"
    );
    this.check_interval_s = this.global_state.ggs_db.get(
      "settings",
      "aprs_fi",
      "check_interval_s"
    );
    // this.check_interval_s = 10;
    this.connection_timeout_s = this.check_interval_s * 2;
  }

  #sendApiRequest() {
    console.log("sending api request to aprs.fi");
    this.name = this.name.toUpperCase();
    this.global_state.debug("requesting aprs.fi data for " + this.name);
    return superagent
      .get(APRS_FI_URL)
      .query({
        name: this.name,
        what: "loc",
        apikey: this.apikey,
        format: "json",
      }) // query string
      .set("Accept", "application/json")
      .set("User-Agent", USER_AGENT)
      .end((err, res) => {
        if (err) {
          this.global_state.error("aprs_fi Error: " + err);
          this.connection_status = "error_1";
          this.failed += 1;
          return;
        }

        this.is_connected = true;
        if (!res.body.found || !res.body.entries) {
          this.global_state.error(
            "aprs_fi error_2: " + JSON.stringify(res.body)
          );
          this.connection_status = "error_2";
          this.failed += 1;
          return;
        }

        if (res.body.found <= 0) {
          this.global_state.warning(
            "No APRS data found for " + this.name + " on aprs.fi."
          );
          this.connection_status = "no_data_found";
          this.failed += 1;
          return;
        }

        this.most_recent_packet = res.body.entries[0];
        this.connection_status = "connected";
        this.global_state.debug(
          "aprs_fi: " + JSON.stringify(this.most_recent_packet)
        );
        this.failed = 0;
        this.global_state.database.addAprsFiData(this.most_recent_packet);
      });
  }
};
