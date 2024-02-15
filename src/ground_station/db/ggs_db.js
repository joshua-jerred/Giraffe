const fs = require("fs");
const file_paths = require("../file_paths");

const loadMetaData = require("../metadata/metaLoader");

const db_tables = ["settings", "data"];

class GgsDataBase {
  constructor() {
    this.db = {};
    this.file_path = file_paths.GGS_CONFIG_JSON;
    this.load();
  }

  load() {
    file_paths.prepareFileSystem();
    if (fs.existsSync(this.file_path) === false) {
      fs.writeFileSync(this.file_path, "{}");
    }

    try {
      this.db = JSON.parse(fs.readFileSync(this.file_path));
    } catch (err) {
      fs.writeFileSync(this.file_path, JSON.stringify({}, null, 2));
    }

    // Iterate through the tables (settings, data)
    for (var i in db_tables) {
      if (this.db[db_tables[i]] === undefined) {
        this.db[db_tables[i]] = {};
      }

      // Iterate through the categories
      for (var category in loadMetaData("ggs", db_tables[i])) {
        let subcategories_meta = loadMetaData("ggs", db_tables[i], category);
        // Don't load subcategories that are not supposed to be in the db
        if ("NOT_IN_DB" in subcategories_meta) {
          continue;
        }

        if (this.db[db_tables[i]][category] === undefined) {
          this.db[db_tables[i]][category] = {};
        }

        // Iterate through the subcategories

        for (var subcategory in subcategories_meta) {
          if (this.db[db_tables[i]][category][subcategory] === undefined) {
            this.db[db_tables[i]][category][subcategory] =
              subcategories_meta[subcategory].default;
          }
        }
      }
    }

    this.save();
  }

  save() {
    fs.writeFileSync(this.file_path, JSON.stringify(this.db, null, 2));
  }

  get(resource, category = null, key = null) {
    let loaded = null;
    if (!category) {
      loaded = this.db[resource];
    } else if (!key) {
      loaded = this.db[resource][category];
    } else {
      loaded = this.db[resource][category][key];
    }

    if (loaded === undefined || loaded === null) {
      return null;
    } else {
      return loaded;
    }
  }

  setKey(resource, category, key, value, save = true) {
    this.db[resource][category][key] = value;
    if (save) {
      this.save();
    }
  }

  setCategory(resource, category, value) {
    this.db[resource][category] = value;
    this.save();
  }

  setResource(resource, value) {
    // settings / data
    this.db[resource] = value;
    this.save();
  }

  resetCategory(resource, category) {
    if (this.db[resource] === undefined) {
      return false;
    } else if (this.db[resource][category] === undefined) {
      return false;
    } else {
      this.db[resource][category] = {};
      this.save();
      this.load();
      return true;
    }
  }

  resetDb() {
    this.db = {};
    this.save();
    this.load();
  }
}

module.exports = GgsDataBase;

var db = new GgsDataBase();
