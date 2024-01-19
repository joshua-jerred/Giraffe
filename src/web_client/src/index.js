import React from "react";
import ReactDOM from "react-dom/client";
import reportWebVitals from "./reportWebVitals";

import { GwsGlobalContextProvider } from "./GlobalContext";
// import { GgsApiContextProvider } from "./api_interface/ggs_api";

import "font-awesome/css/font-awesome.min.css"; // used for nav bar icons only
import "./style/global.css";

import App from "./App";

const root = ReactDOM.createRoot(document.getElementById("root"));
root.render(
  <React.StrictMode>
    <GwsGlobalContextProvider>
      {/* <GgsApiContextProvider> */}
      <App />
      {/* </GgsApiContextProvider> */}
    </GwsGlobalContextProvider>
  </React.StrictMode>
);

// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
