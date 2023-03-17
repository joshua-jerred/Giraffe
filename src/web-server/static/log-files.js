log_files_loaded = false;

async function updateFileList(log_files_json) {
  let data_log_files = log_files_json['data'];
  let error_log_files = log_files_json['error'];
  let data_files_element = document.querySelector('#data-log-files');
  let error_files_element = document.querySelector('#error-log-files');
  data_files_element.innerHTML = '';
  error_files_element.innerHTML = '';

  for (let i = 0; i < data_log_files.length; i++) {
    addFile(data_files_element, data_log_files[i], 'data');
  }

  for (let i = 0; i < error_log_files.length; i++) {
    addFile(error_files_element, error_log_files[i], 'error');
  }
}

async function rotateLogFiles() {
  sendCommand('cmd/dat/rlf/');
  setTimeout(function(){sendCommand('cmd/dat/ufl/')}, 500); // Update file list
  log_files_loaded = false;
  setTimeout(getFileLists, 1000);
}

async function DeleteFile() {
  let file = this.id;
  let command = '';
  if (this.classList.contains('data')) {
    command = 'cmd/dat/ddf/' + file;
  } else if (this.classList.contains('error')) {
    command = 'cmd/dat/def/' + file;  
  }
  sendCommand(command);
  setTimeout(function(){sendCommand('cmd/dat/ufl/')}, 500); // Update file list
  log_files_loaded = false;
  setTimeout(getFileLists, 1000);
}

async function addFile(list_element, file, type) {
  let new_li = document.createElement('li');
  let delete_button = document.createElement('button');
  delete_button.innerHTML = 'Delete';
  delete_button.id = file.split('.json')[0];
  delete_button.classList.add('gfs-button');
  delete_button.classList.add(type)
  new_li.innerHTML = file;
  delete_button.addEventListener('click', DeleteFile);
  new_li.appendChild(delete_button);
  list_element.appendChild(new_li);
}

async function getFileLists() {
  if (connection_status) {
    let log_files = await fetch('/api/log-files')
      .then((response) => response.json())
      .then((data) => { return data })
      .catch((error) => { return false });
    if (log_files['data']) {
      log_files_loaded = true;
    } else {
      log_files_loaded = false;
    }
    if (log_files['error']) {
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
  document.querySelector('#rotate-log-files-button').addEventListener('click', rotateLogFiles);
});