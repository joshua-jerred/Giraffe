function loadSetup() {
  initEditable("#setup-editable", "/gws/setup");
}

document.addEventListener("StatusUpdate", function () {
  console.log(gws_status, gfs_status, ggs_status, mode);
  if (gws_status == true) {
    loadSetup();
  }
});