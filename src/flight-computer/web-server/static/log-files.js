log_files_loaded = false;

async function updateFileList(log_files_json) {
  let data_log_files = log_files_json['data'];
  let error_log_files = log_files_json['error'];
  let data_files_element = document.querySelector('#data-log-files');
  let error_files_element = document.querySelector('#error-log-files');
  data_files_element.innerHTML = '';
  error_files_element.innerHTML = '';

  for (let i = 0; i < data_log_files.length; i++) {
    let new_li = document.createElement('li');
    new_li.innerHTML = data_log_files[i];
    data_files_element.appendChild(new_li);
  }

  for (let i = 0; i < error_log_files.length; i++) {
    let new_li = document.createElement('li');
    new_li.innerHTML = error_log_files[i];
    error_files_element.appendChild(new_li);
  }
}

async function getFileLists() {
  if (connection_status) {
    let log_files = await fetch('/api/log-files')
      .then((response) => response.json())
      .then((data) => { return data })
      .catch((error) => { return false });
    if (log_files['data']) {
      console.log(log_files['data']);
      log_files_loaded = true;
    } else {
      log_files_loaded = false;
    }
    if (log_files['error']) {
      console.log(log_files['error']);
      if (log_files_loaded == true) {
        updateFileList(log_files);
        setTimeout(getFileLists, 20000);
        return;
      }
    } else {
      log_files_loaded = false;
    }
  }
  setTimeout(getFileLists, 1000);
}

window.addEventListener("DOMContentLoaded", function () {
  getFileLists();
});