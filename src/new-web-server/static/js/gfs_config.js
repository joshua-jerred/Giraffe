var config_info_json = {};
var config_loader = null;

// -----------------------------------------------------------------------------
// Edit Functionality
function ExpandAllToggle(expand_all_button) {
  let all_expanded = document.querySelectorAll(".expanded-row");
  if (all_expanded.length > 0) {
    for (let i = 0; i < all_expanded.length; i++) {
      all_expanded[i].classList.remove("expanded-row");
      all_expanded[i].style.display = "none";
    }
    expand_all_button.innerText = "+";
  } else {
    let all_rows = document.querySelectorAll(".extension-details");
    for (let i = 0; i < all_rows.length; i++) {
      all_rows[i].classList.add("expanded-row");
      all_rows[i].style.display = "block";
    }
    expand_all_button.innerText = "-";
  }
}

function ExpandExtensionRow(row) {
  let expand_all_button = document.getElementById("expand-all-button");
  let num_expanded = document.querySelectorAll(".expanded-row").length;
  
  console.log("Expanding row: " + row.id);
  if (row.classList.contains("expanded-row")) {
    row.classList.remove("expanded-row");
    row.style.display = "none";
    num_expanded--;
  } else {
    row.classList.add("expanded-row");
    row.style.display = "block";
    num_expanded++;
  }

  if (num_expanded == 0) {
    expand_all_button.innerText = "+";
  } else {
    expand_all_button.innerText = "-";
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// For loading the configuration from GFS

function InsertData(cfg_item, data) {
  let cfg_data = document.getElementById(cfg_item).querySelector(".cfg-data");
  cfg_data.innerText = data;
}

function ParseGeneralData(data) {
  InsertData("project-name", data["project-name"]);
  InsertData("main-board-type", data["main-board-type"]);
  InsertData("starting-procedure", data["starting-procedure"]);
}

function ParseDebuggingData(data) {
  InsertData("debug-mode-print-errors", data["debug-mode-print-errors"]);
  InsertData("console-enabled", data["console-enabled"]);
  InsertData("console-update-interval", data["console-update-interval"]);
  InsertData("web-socket-enabled", data["web-socket-enabled"]);
  InsertData("web-socket-port", data["web-socket-port"]);
}

function ParseExtensionData(data) {
  for (let i = 0; i < data.length; i++) {
    let extension = data[i];
    let id = extension["id"];
    let name = extension["name"];
    let type = extension["type"];
    let category = extension["category"];
    let ext_interface = extension["interface"];
    let update_interval = extension["update-interval"];
    let flight_critical = extension["flight-critical"];

    let table_row = document.createElement("tr");
    table_row.id = "extension-" + id;

    let table_row_data = "<td>" + id + "</td>" + "<td>" + name + "</td>"
      + "<td>" + type + "</td>" + "<td>" + category + "</td>"
      + "<td>" + ext_interface + "</td>" + "<td>" + update_interval + "</td>"
      + "<td>" + flight_critical + "</td>";

    table_row.innerHTML = table_row_data;

    let details = "test";

    let details_row = document.createElement("tr");
    let details_row_data = document.createElement("td");
    details_row_data.colSpan = 7;
    details_row_data.classList.add("extension-details");
    details_row_data.innerHTML = details;
    details_row.appendChild(details_row_data);

    table_row.addEventListener("click", function () {
      ExpandExtensionRow(details_row_data);
    });

    document.getElementById("extension-list").appendChild(table_row);
    document.getElementById("extension-list").appendChild(details_row);
  }
}

function loadGfsConfiguration() {
  fetch("/gfs/config", {
    method: "GET",
    headers: {
      "Content-Type": "application/json",
    }
  }).then(response => response.json())
    .then((response) => {
      ParseGeneralData(response["general"]);
      ParseDebuggingData(response["debugging"]);
      ParseExtensionData(response["extensions"]);
      RemoveLoader(config_loader);
    }).catch((error) => {
      StopLoader(config_loader, "Failed to load configuration from GFS: " + error);
    });
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Everything below is for pulling the configuration options from a json file.

function genExtension(extension_info) { // For "extension" options
  
}

// Pass in an object from the json file and it'll generate the html for it.
function genCfgItem(config_info, tool_tip_dir) {
  let element = document.createElement("div");
  let type = "cfg"+config_info["cfg-type"];
  element.classList.add("cfg-item");
  element.classList.add("tooltip");
  element.classList.add(type);
  element.id = config_info["cfg-item"];

  let cfg_name = document.createElement("span");
  cfg_name.classList.add("cfg-name");
  cfg_name.innerText = config_info["name"];

  let cfg_data = document.createElement("span");
  cfg_data.classList.add("cfg-data");
  cfg_data.innerText = "No Data";

  let cfg_tooltip_text = document.createElement("span");
  cfg_tooltip_text.classList.add("tooltiptext");
  cfg_tooltip_text.classList.add(tool_tip_dir);

  let cfg_tooltip_text_content = document.createElement("p");
  cfg_tooltip_text_content.innerText = config_info["description"];

  cfg_tooltip_text.appendChild(cfg_tooltip_text_content);
  element.appendChild(cfg_name);
  element.appendChild(cfg_data);
  element.appendChild(cfg_tooltip_text);
  return element;
}

function genCfgSection(json_list, section_id, tool_tip_dir) {
  let section = document.getElementById(section_id);
  for (let i = 0; i < json_list.length; i++) {
    section.appendChild(genCfgItem(json_list[i], tool_tip_dir));
  }
}

function parseConfigInfo() {
  genCfgSection(config_info_json["general"], "cfg-general", "top"); // General
  genCfgSection(config_info_json["debugging"], "cfg-debug", "top"); // Debugging

  let extensions = config_info_json["extensions"]; // Extensions

  genCfgSection(config_info_json["telemetry"]["general"], "cfg-telemetry-general", "top"); // Telemetry
  genCfgSection(config_info_json["telemetry"]["aprs"], "cfg-telemetry-aprs", "top"); // APRS
  genCfgSection(config_info_json["telemetry"]["sstv"], "cfg-telemetry-sstv", "top"); // SSTV
  genCfgSection(config_info_json["telemetry"]["data-packets"], "cfg-telemetry-data-packets", "top"); // Data Packets
}

function getConfigInfo() {
  fetch("/gfs_config_info.json", {
    method: "GET",
    headers: {
      "Content-Type": "application/json",
    }
  })
    .then(response => response.json())
    .then((response) => {
      config_info_json = response;
      parseConfigInfo();
      UpdateLoaderText(config_loader, "Loading GFS Configuration...");
      loadGfsConfiguration();
    }).catch((error) => {
      StopLoader(config_loader, "Error Loading Configuration Info: " + error);
    });
}

window.addEventListener("DOMContentLoaded", function () {
  config_loader = InsertLoader("Loading Configuration Info...");
  getConfigInfo();

  let expand_all_button = document.getElementById("expand-all-button");
  expand_all_button.addEventListener("click", function () {
    ExpandAllToggle(expand_all_button);
  });
});