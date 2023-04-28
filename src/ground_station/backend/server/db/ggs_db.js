const fs = require("fs");
const path = require("path");

const loadMetaData = require("../metadata/metaLoader");
const file_name = "ggs_setup.json";

const db_tables = ["settings", "data"];

class GgsDataBase {
  constructor() {
    this.db = {};
    this.file_path = path.join(__dirname, "..", "db", file_name);
    this.load();
  }

  load() {
    if (fs.existsSync(this.file_path) === false) {
      fs.writeFileSync(this.file_path, "{}");
      console.log("Created new database file: " + this.file_path);
    }

    try {
      this.db = JSON.parse(fs.readFileSync(this.file_path));
    } catch (err) {
      console.log("Error reading database file: " + this.file_path);
      console.log(err);
      fs.writeFileSync(this.file_path, JSON.stringify({}, null, 2));
    }

    // Iterate through the tables
    for (var i in db_tables) {
      if (this.db[db_tables[i]] === undefined) {
        this.db[db_tables[i]] = {};
      }

      // Iterate through the categories
      for (var category in loadMetaData("ggs", db_tables[i])) {
        if (this.db[db_tables[i]][category] === undefined) {
          this.db[db_tables[i]][category] = {};
        }

        // Iterate through the subcategories
        for (var subcategory in loadMetaData("ggs", db_tables[i], category)) {
          if (this.db[db_tables[i]][category][subcategory] === undefined) {
            this.db[db_tables[i]][category][subcategory] = loadMetaData(
              "ggs",
              db_tables[i],
              category,
              subcategory
            )["default"];
          }
        }
      }
    }

    this.save();
  }

  save() {
    fs.writeFileSync(this.file_path, JSON.stringify(this.db, null, 2));
  }

  get(table, category = null, key = null) {
    if (category === null || category === undefined) {
      return this.db[table];
    } else if (key === null) {
      return this.db[table][category];
    } else {
      return this.db[table][category][key];
    }
  }

  setKey(table, category, key, value) {
    this.db[table][category][key] = value;
    this.save();
  }

  setCategory(table, category, value) {
    this.db[table][category] = value;
    this.save();
  }

  setTable(table, value) {
    // settings / data
    this.db[table] = value;
    this.save();
  }

  resetDb(table) {
    this.db = {};
    this.save();
    this.load();
  }
}

module.exports = GgsDataBase;

var db = new GgsDataBase();
