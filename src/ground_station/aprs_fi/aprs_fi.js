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
    this.is_connected = true;

    this.#updateSettings();
  }

  updateAndGetStatus() {
    if (!this.enabled) {
      return "disabled";
    }
    let seconds_elapsed = (new Date() - this.time_of_last_get_attempt) / 1000;

    console.log(seconds_elapsed);
    if (seconds_elapsed > this.check_interval_s) {
      console.log("requesting aprs.fi data");
      this.time_of_last_get_attempt = new Date();
      this.#sendApiRequest();
    }

    return this.is_connected ? "connected" : "error";
  }

  #updateSettings() {
    this.enabled = this.global_state.ggs_db.get(
      "settings",
      "aprs_fi",
      "enabled"
    );
    this.name = this.global_state.ggs_db.get("settings", "aprs_fi", "name");
    this.apikey = this.global_state.ggs_db.get("settings", "aprs_fi", "apikey");
    this.check_interval_s = this.global_state.ggs_db.get(
      "settings",
      "aprs_fi",
      "check_interval_s"
    );
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
          console.log("aprs_fi Error: ", err);
          this.is_connected = false;
          return;
        }

        this.is_connected = true;
        console.log("aprs_fi Response: ", res.body);
      });
  }
};
