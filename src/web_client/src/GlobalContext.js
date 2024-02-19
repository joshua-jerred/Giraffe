import React from "react";
import Alerter from "./core/alerter";

export const GwsGlobal = React.createContext("");
export const GwsGlobalContextProvider = ({ children }) => {
  // ------ LOCAL STORAGE ------
  function save(key, value) {
    window.localStorage.setItem(key, value);
  }
  function load(key) {
    return window.localStorage.getItem(key);
  }

  const [clientDarkTheme, setClientDarkTheme] = React.useState(
    load("client_dark_theme") !== "false"
  );
  const [clientName, setClientName] = React.useState(
    load("client_name") || "not_set"
  );
  const [ggsAddress, setGgsAddress] = React.useState(
    load("client_ggs_address") || "http://localhost:7892"
  );
  const [connectionInterval, setConnectionInterval] = React.useState(
    load("connection_interval") || 3000 // ms
  );

  React.useEffect(() => {
    save("client_dark_theme", clientDarkTheme);
    save("client_name", clientName);
    save("client_ggs_address", ggsAddress);
    save("connection_interval", connectionInterval);
  }, [clientDarkTheme, clientName, ggsAddress, connectionInterval]);

  // ------ STATUSES & ALERTER ------
  const serviceStatusesDefault = {
    telemetry_uplink: "unknown",
    telemetry_downlink: "unknown",
    gfs: "unknown",
    gdl: "unknown",
    aprsfi: "unknown",
    influxdb: "unknown",
  };

  const [serviceStatuses, setServiceStatuses] = React.useState(
    serviceStatusesDefault
  );

  const [alerter] = React.useState(new Alerter());

  const [isGgsConnected, setIsGgsConnected] = React.useState(false);
  const [isGfsTcpConnected, setIsGfsTcpConnected] = React.useState(false);
  const [isGdlConnected, setIsGdlConnected] = React.useState(false);
  const [isDownlinkConnected, setIsDownlinkConnected] = React.useState(false);
  const [isUplinkConnected, setIsUplinkConnected] = React.useState(false);

  // ------ GGS Connection ------
  React.useEffect(() => {
    const intervalUpdateCallback = () => {
      // update the alerter
      alerter.updateAlerts();

      // first validate the update interval
      const MIN_INTERVAL_MS = 1000 * 0.75;
      const MAX_INTERVAL_MS = 1000 * 60;
      if (connectionInterval < MIN_INTERVAL_MS) {
        setConnectionInterval(MIN_INTERVAL_MS);
      } else if (connectionInterval > MAX_INTERVAL_MS) {
        setConnectionInterval(MAX_INTERVAL_MS);
      }

      // check client name
      const re = /^[a-zA-Z_]*$/;
      if (
        !re.test(clientName) ||
        clientName.length < 3 ||
        clientName.length > 15 ||
        clientName === "not_set"
      ) {
        alerter.addAlert(
          "client_name_not_set",
          "Client name not set.",
          connectionInterval + 1000,
          "/setup"
        );
      }

      // check the address and test the connection
      try {
        new URL(ggsAddress);
      } catch (_) {
        alerter.addAlert(
          "ggs_address_invalid.",
          "Client configured GGS address is invalid.",
          connectionInterval + 1000,
          "/setup"
        );
        setIsGgsConnected(false);
        setServiceStatuses(serviceStatusesDefault);
        return; // don't test connection if the address is invalid
      }

      // test the connection
      fetch(`${ggsAddress}/api/status`)
        .then((response) => response.json())
        .then((data) => {
          if (!isGgsConnected) {
            alerter.clearAlert("not_connected_to_ggs");
          }
          setIsGgsConnected(true);
          setServiceStatuses(data);
          setIsGfsTcpConnected(data.gfs === "connected");
          setIsGdlConnected(data.gdl === "connected");
          setIsUplinkConnected(data.telemetry_uplink === "connected");
          setIsDownlinkConnected(data.telemetry_downlink === "connected");
        })
        .catch((error) => {
          alerter.addAlert(
            "not_connected_to_ggs",
            "Not connected to the Ground Station Server.",
            3000,
            "/setup"
          );
          setIsGgsConnected(false);
          setIsGfsTcpConnected(false);
          setIsGdlConnected(false);
          setIsUplinkConnected(false);
          setIsDownlinkConnected(false);
          setServiceStatuses(serviceStatusesDefault);
        });
    };

    intervalUpdateCallback(); // run once on mount

    const interval = setInterval(() => {
      intervalUpdateCallback();
    }, connectionInterval);
    return () => clearInterval(interval);
  }, [ggsAddress]);

  return (
    <GwsGlobal.Provider
      value={{
        clientDarkTheme,
        clientName,
        ggsAddress,
        setClientDarkTheme,
        setClientName,
        setGgsAddress,
        alerter,
        serviceStatuses,
        isGgsConnected,
        isGfsTcpConnected,
        isGdlConnected,
        isDownlinkConnected,
        isUplinkConnected,
      }}
    >
      {children}
    </GwsGlobal.Provider>
  );
};
