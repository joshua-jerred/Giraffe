import React from "react";
import ReactDOM from "react-dom/client";
import App from "./App";
import reportWebVitals from "./reportWebVitals";

import "font-awesome/css/font-awesome.min.css";
import { library } from "@fortawesome/fontawesome-svg-core";
import {
  faSatelliteDish,
  faSatellite,
} from "@fortawesome/free-solid-svg-icons";

import { GwsGlobalContextProvider } from "./GlobalContext";
import { GgsApiContextProvider } from "./api_interface/ggs_api";

library.add(faSatelliteDish, faSatellite);

const root = ReactDOM.createRoot(document.getElementById("root"));
root.render(
  <React.StrictMode>
    <GwsGlobalContextProvider>
      <GgsApiContextProvider>
        <App />
      </GgsApiContextProvider>
    </GwsGlobalContextProvider>
  </React.StrictMode>
);

// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
