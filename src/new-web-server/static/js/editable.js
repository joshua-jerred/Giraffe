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

function injectEditable(editable_object, data) {
  for (let key in data) {
    let value = data[key];
    if (WhatJsonType(value) == "object") {
      injectEditable(editable_object, value);
    } else {
      
      elm = editable_object.querySelector("#" + key);
      if (elm != null) {
        elm.querySelector("span").innerText = value;
        console.log("#" + key, value);
      }
    }
  }
}

async function initEditable(selector_to_editable, get_path) {
  let editable = document.querySelector(selector_to_editable);
  
  if (editable == null || !editable.classList.contains("editable")) {
    console.log("initEditable: no editable found");
    return;
  }

  await fetch(get_path, {
    method: "GET",
    headers: {
      "Content-Type": "application/json",
    }})
  .then((response) => response.json())
  .then((data) => {
      console.log("initEditable: got data", data);
      injectEditable(editable, data);
  })
}