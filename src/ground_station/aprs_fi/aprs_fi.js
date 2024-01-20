const version_info = require("../../common/version");
const superagent = require("superagent");
const sqlite3 = require("sqlite3").verbose();

const USER_AGENT =
  `Giraffe/${version_info.version}-${version_info.stage} ` +
  `(Giraffe Ground Station, GGS; +${version_info.git_repo}; ` +
  `${version_info.git_hash}) Node.js/${process.version}`;

APRS_FI_URL = "https://api.aprs.fi/api/get";
// APRS_FI_URL = "http://localhost:8080/api/get";

module.exports = class AprsFi {
  constructor() {
    // must include the SSID
    this.name = "KD9GDC-10";
    this.apikey = "87299.Hf98Lly9Sm8qIuvP";
    this.db = new sqlite3.Database("../db/ggs.sqlite3");

    this.packets = [];
  }

  setApiKey(apikey) {
    this.apikey = apikey;
  }

  setName(name) {
    this.name = name;
  }

  get() {
    this.name = this.name.toUpperCase();
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
        console.log("Response: ", res.body);
        // console.log("Response: ", res);
        // console.log("Error: ", err);
        // err ? console.log(err) : console.log(res.body);
        return res.body;
      });
  }

  getSampleEmptyResponse() {
    return {};
  }

  getSampleSimpleResponse() {
    return {
      command: "get",
      result: "ok",
      what: "loc",
      found: 1,
      entries: [
        {
          class: "a",
          name: "K7DPO-9",
          type: "l",
          time: "1705710946",
          lasttime: "1705710946",
          lat: "36.80250",
          lng: "-114.07550",
          altitude: 487,
          course: 179,
          speed: 14.816,
          symbol: "/j",
          srccall: "K7DPO-9",
          dstcall: "S6TXQU",
          mice_msg: "101",
          path: "WIDE1-1,qAR,UTAH",
        },
      ],
    };
  }

  getAnotherSample() {
    return {
      command: "get",
      result: "ok",
      what: "loc",
      found: 1,
      entries: [
        {
          class: "a",
          name: "KD9GDC-10",
          type: "l",
          time: "1705713215",
          lasttime: "1705713215",
          lat: "37.10850",
          lng: "-113.56933",
          altitude: 812.292,
          symbol: "/[",
          srccall: "KD9GDC-10",
          dstcall: "APDR16",
          comment: "Testing APRS-IS via APRSDroid",
          path: "TCPIP*,qAC,T2CZECH",
        },
      ],
    };
  }
};
