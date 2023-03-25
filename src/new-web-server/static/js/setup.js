let info_read = false;

function webConfigSave(message) {
  console.log("web config saved");
}

function loadSetup() {
  
  if (!info_read) {
    fetch("/gws/setup").then(response => response.json()).then(data => {
      injectData("#gws-version", data["version"]);

      if (data["web-config"]["developer-mode"]) {
        console.log("developer mode");
        data = data["pages"];
        injectData("#home-page", data["home-page"]);
        injectData("#encoding", data["encoding"]);
        injectData("#static-dir", data["static-dir"]);
        injectData("#jinja-dirs", data["jinja-dirs"]);
        injectData("#allowed-extensions", data["allowed-extensions"]);
      } else {
        document.querySelector("#developer-info").style.display = "none";
      }
    });
  }
}

document.addEventListener("StatusUpdate", function () {
  console.log(gws_status, gfs_status, ggs_status, mode);
  if (gws_status == true) {
    loadSetup();
    initEditable("#setup-editable", "/gws/setup/web-config", "/gws/setup/web-config", webConfigSave);
  }
});