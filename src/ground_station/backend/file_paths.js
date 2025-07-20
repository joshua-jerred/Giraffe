const fs = require("fs");
const os = require("os");

const USER_DIR = os.homedir();
const MAIN_DIR_NAME = "giraffe";
const MAIN_DIR_PATH = USER_DIR + "/" + MAIN_DIR_NAME;

const GGS_CONFIG_JSON = MAIN_DIR_PATH + "/ggs_config.json";
const GGS_SQLITE_DB = MAIN_DIR_PATH + "/ggs.sqlite3";
const GGS_IMAGE_DIR = MAIN_DIR_PATH + "/ggs_images";

function prepareFileSystem() {
  if (!fs.existsSync(MAIN_DIR_PATH)) {
    console.log("Creating main data directory: " + MAIN_DIR_PATH);
    try {
      fs.mkdirSync(MAIN_DIR_PATH);
    } catch (err) {
      console.log("Error creating main data directory: " + MAIN_DIR_PATH);
      console.log(err);
      // process.exit(1);
    }
  }
  if (!fs.existsSync(GGS_IMAGE_DIR)) {
    console.log("Creating image directory: " + GGS_IMAGE_DIR);
    try {
      fs.mkdirSync(GGS_IMAGE_DIR);
    } catch (err) {
      console.log("Error creating image directory: " + GGS_IMAGE_DIR);
      console.log(err);
      // process.exit(1);
    }
  }
}

module.exports = {
  prepareFileSystem,
  GGS_CONFIG_JSON,
  GGS_SQLITE_DB,
  GGS_IMAGE_DIR,
};
