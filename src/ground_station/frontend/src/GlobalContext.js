import React from "react";
import Alerter from "./core/alerter";
import { useStorageState } from "./core/LocalStorageState";

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
  const [unsafeMode, setUnSafeMode] = React.useState(
    load("unsafe_mode") === "true" || false
  );
  const [ggsAddress, setGgsAddress] = React.useState(
    load("client_ggs_address") || "http://localhost:7892"
  );
  const [connectionInterval, setConnectionInterval] = React.useState(
    load("connection_interval") || 3000 // ms
  );
  const [navExpanded, setNavExpanded] = React.useState(
    load("nav_expanded") === "true" || false
  );
  const [showVerboseClock, setShowVerboseClock] = useStorageState(
    "status_bar_mission_clock_show_milliseconds",
    true
  );

  React.useEffect(() => {
    localStorage.setItem("nav_expanded", navExpanded);
  }, [navExpanded]);

  React.useEffect(() => {
    save("client_dark_theme", clientDarkTheme);
    save("client_name", clientName);
    save("client_ggs_address", ggsAddress);
    save("connection_interval", connectionInterval);
    save("unsafe_mode", unsafeMode);
  }, [clientDarkTheme, clientName, ggsAddress, connectionInterval, unsafeMode]);

  // ------ STATUSES & ALERTER ------
  const serviceStatusesDefault = {
    telemetry_uplink: "n/d",
    telemetry_downlink: "n/d",
    gfs: "n/d",
    gdl: "n/d",
    fsa: "n/d", // Connected to the agent
    fsa_gfs: "n/d", // The agent is connected to the GFS
    aprs_is: "n/d",
    aprs_fi: "n/d",
    influxdb: "n/d",
  };

  const [serviceStatuses, setServiceStatuses] = React.useState(
    serviceStatusesDefault
  );

  const [alerter] = React.useState(new Alerter());

  const [isGgsConnected, setIsGgsConnected] = React.useState(false);
  const [isGfsTcpConnected, setIsGfsTcpConnected] = React.useState(false);
  const [isGdlConnected, setIsGdlConnected] = React.useState(false);
  const [isFsaConnected, setIsFsaConnected] = React.useState(false);
  const [fsaGfsStatus, setFsaGfsStatus] = React.useState(false);
  const [isDownlinkConnected, setIsDownlinkConnected] = React.useState(false);
  const [isUplinkConnected, setIsUplinkConnected] = React.useState(false);

  const [flightData, setFlightData] = React.useState({});
  const [inActiveFlight, setInActiveFlight] = React.useState(false);

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
          1000,
          "/setup"
        );
      }

      // check if in unsafe mode
      if (unsafeMode) {
        alerter.addAlert("unsafe_mode", "Unsafe mode enabled.", 1000, "/setup");
      }

      // check the address and test the connection
      try {
        new URL(ggsAddress);
      } catch (_) {
        alerter.addAlert(
          "ggs_address_invalid.",
          "Client configured GGS address is invalid.",
          3000,
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
          // console.log("test connection");
          if (!isGgsConnected) {
            alerter.clearAlert("not_connected_to_ggs");
          }
          setIsGgsConnected(true);
          setServiceStatuses(data);
          setIsGfsTcpConnected(data.gfs === "connected");
          setIsGdlConnected(data.gdl === "connected");
          setIsFsaConnected(data.fsa === "connected");
          // setFsaGfsStatus(data.fsa_gfs === "connected");
          setIsUplinkConnected(data.telemetry_uplink === "connected");
          setIsDownlinkConnected(data.telemetry_downlink === "connected");

          if (data.gdl !== "connected" && data.gdl !== "disabled") {
            alerter.addAlert(
              "not_connected_to_gdl",
              "The Ground Station Server is not connected to the Data Link.",
              0,
              "/setup"
            );
          } else {
            alerter.clearAlert("not_connected_to_gdl");
          }

          // if (data.fsa !== "connected") {
          //   alerter.addAlert(
          //     "not_connected_to_fsa",
          //     "The Ground Station Server is not connected to the Flight System Agent.",
          //     0,
          //     "/setup"
          //   );
          // } else {
          //   alerter.clearAlert("not_connected_to_fsa");
          // }
        })
        .catch((error) => {
          alerter.addAlert(
            "not_connected_to_ggs",
            "Not connected to the Ground Station Server.",
            0,
            "/setup"
          );
          setIsGgsConnected(false);
          setIsGfsTcpConnected(false);
          setIsGdlConnected(false);
          setIsUplinkConnected(false);
          setIsDownlinkConnected(false);
          setServiceStatuses(serviceStatusesDefault);
        });

      // get flight data if connected
      fetch(`${ggsAddress}/api/flight_data/data?category=general`)
        .then((response) => response.json())
        .then((json_data) => {
          // console.log("Flight data", json_data);
          setFlightData(json_data.values);

          if (json_data.values.hasOwnProperty("flight_phase")) {
            const phase = json_data.values.flight_phase.toLowerCase();
            if (phase === "ascent" || phase === "descent") {
              setInActiveFlight(true);
            } else {
              setInActiveFlight(false);
            }
          }

          if (
            json_data.values.hasOwnProperty("simulator_mode") &&
            json_data.values.simulator_mode
          ) {
            alerter.addAlert(
              "simulator_mode.",
              "This GFS build is running in simulator mode.",
              3000,
              ""
            );
          }
        })
        .catch((error) => {
          console.error("Error getting flight data", error);
        });

      // Flight System Agent data
      // fetch(`${ggsAddress}/api/fsa/data?category=status&include=values`)
      //   .then((response) => response.json())
      //   .then((json_data) => {
      //     setFlightSystemAgentData(json_data.values);
      //   })
      //   .catch((error) => {
      //     console.error("Error getting FSA data", error);
      //   });
    };

    intervalUpdateCallback(); // run once on mount

    const interval = setInterval(() => {
      intervalUpdateCallback();
    }, connectionInterval);
    return () => clearInterval(interval);

    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [ggsAddress, unsafeMode]);

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
        flightData,
        navExpanded,
        setNavExpanded,
        unsafeMode,
        setUnSafeMode,
        showVerboseClock,
        setShowVerboseClock,
        inActiveFlight,
      }}
    >
      {children}
    </GwsGlobal.Provider>
  );
};
