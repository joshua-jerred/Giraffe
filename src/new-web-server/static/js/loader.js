function InsertLoader(text = "Loading...") {
  let loader_box = document.querySelector("#loader-box");

  let loader = document.createElement("div");
  loader.classList.add("loader");

  let loader_text = document.createElement("div");
  loader_text.id = "loader-text";
  loader_text.innerText = text;

  loader_box.appendChild(loader);
  loader_box.appendChild(loader_text);

  return loader;
}

function UpdateLoaderText(loader, text) {
  document.querySelector("#loader-text").innerText = text;
}

function StopLoader(loader, text = "Error") {
  document.querySelector(".loader").remove();
  document.querySelector("#loader-text").innerText = text;
}

async function RemoveLoader(loader) {
  document.querySelector(".loader").remove();
  document.querySelector("#loader-text").innerText = "Done.";
  setTimeout(function () {
    document.querySelector("#loader-box").style.display = "none";
  }, 500);
}
