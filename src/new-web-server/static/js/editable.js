function WhatJsonType(obj) {
  if (obj === null) return "null";
  if (obj === undefined) return "undefined";
  if (Array.isArray(obj)) return "array";
  if (typeof obj === "object") return "object";
  if (typeof obj === "string") return "string";
  if (typeof obj === "number") return "number";
  if (typeof obj === "boolean") return "boolean";
  return "unknown";
}

function saveEditable(editable_object, get_path, post_path, save_callback) {
  let data = {};
  for (let div of editable_object.querySelectorAll("div")) {
    let input = div.querySelector("input");
    if (input != null) {
      let key = div.id;
      if (input.type == "checkbox") {
        var value = input.checked;
      } else {
        var value = input.value;
      }
      data[key] = value;
    }
  }

  fetch(post_path, {
    method: "POST",
    body: JSON.stringify(data),
    headers: {
      "Content-Type": "application/json",
    }
  }).then((response) => {
    if (response.ok) {
      for (let div of editable_object.querySelectorAll("div")) {
        let input = div.querySelector("input");
        if (input != null) {
          let span = document.createElement("span");
          input.replaceWith(span);
        }
      }
      editable_object.querySelector(".save-button").remove();
      editable_object.classList.remove("edit-mode");
      editable_object.classList.remove("injected");
      initEditable("#" + editable_object.id, get_path, post_path);
      if (save_callback != null) {
        save_callback(response);
      }
    }
  }).catch((error) => {
    console.log("saveEditable: error");
  });
}

function editMode(editable_object, get_path, post_path, save_callback = null) {
  let edit_button = editable_object.querySelector(".edit-button");
  let save_button = document.createElement("button");
  save_button.innerText = "Save";
  save_button.classList.add("save-button");
  save_button.addEventListener("click", function () {
    saveEditable(editable_object, get_path, post_path, save_callback);
  });
  edit_button.replaceWith(save_button);
  editable_object.classList.add("edit-mode");
  for (let div of editable_object.querySelectorAll("div")) {
    let span = div.querySelector("span");
    if (span != null) {
      let previous_value = span.innerText;
      let input = document.createElement("input");
      if (!isNaN(previous_value) && previous_value != "") {
        input.type = "number";
      } else if (previous_value == "true" || previous_value == "false") {
        input.type = "checkbox";
        input.checked = (previous_value == "true");
      } else {
        input.type = "text";
      }
      input.value = previous_value;
      span.replaceWith(input);
    }
  }
}

function injectEditable(editable_object, data) {
  let injected = false;
  for (let key in data) {
    let value = data[key];
    if (WhatJsonType(value) == "object") {
      injectEditable(editable_object, value);
    } else {

      elm = editable_object.querySelector("#" + key);
      if (elm != null) {
        elm.querySelector("span").innerText = value;
        injected = true;
      }
    }
  }

  if (injected) {
    editable_object.classList.add("injected");
  }
}



async function initEditable(selector_to_editable, get_path, post_path, save_callback = null) {
  let editable = document.querySelector(selector_to_editable);

  if (editable == null || !editable.classList.contains("editable")) {
    console.log("initEditable: no editable found");
    return;
  }

  if (editable.classList.contains("injected")) {
    console.log("initEditable: already injected");
    return;
  }

  await fetch(get_path, {
    method: "GET",
    headers: {
      "Content-Type": "application/json",
    }
  })
    .then((response) => response.json())
    .then((data) => {
      injectEditable(editable, data);

      let edit_button = document.createElement("button");
      edit_button.innerText = "Edit";
      edit_button.classList.add("edit-button");
      edit_button.addEventListener("click", function () {
        editMode(editable, get_path, post_path, save_callback);
      });
      editable.appendChild(edit_button);

    })
}

function injectData(selector, data) {
  let div = document.querySelector(selector);
  if (div == null) {
    console.log("injectData: no div found " + selector);
  }
  let span = div.querySelector("span");
  if (span == null) {
    console.log("injectData: no span found " + selector);
  }
  span.innerText = data;
}

function inject(selector, json) {
  let data = json[selector];
  console.log(selector, data);
  if (data == null) {
    console.log("injectDataFromJson: no data found " + selector);
  }
  injectData("#" + selector, data);
}