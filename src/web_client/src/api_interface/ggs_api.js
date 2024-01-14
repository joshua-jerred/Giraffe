import { useState, createContext, useContext, useEffect } from "react";
import { GwsGlobal } from "../GlobalContext";

export const GGS_API = createContext("");

export const GgsApiContextProvider = ({ children }) => {
  const { ggsAddress, clientName } = useContext(GwsGlobal);

  const [ggsConnectionStatus, setGgsConnectionStatus] = useState(false);

  const [giraffeStatus, setGiraffeStatus] = useState({
    telemetry_uplink: "unknown",
    telemetry_downlink: "unknown",
    gfs: "unknown",
    gdl: "unknown",
    aprsfi: "unknown",
    influxdb: "unknown",
  });

  const CHECK_CONNECTION_INTERVAL = 2500;
  useEffect(() => {
    const interval = setInterval(() => {
      // console.log("Logs every second" + ggsAddress);
      fetch(`http://${ggsAddress}/api/status`)
        .then((response) => response.json())
        .then((data) => {
          setGiraffeStatus(data);
          setGgsConnectionStatus(true);
        })
        .catch((error) => {
          setGgsConnectionStatus(false);
        });
    }, CHECK_CONNECTION_INTERVAL);
    return () => clearInterval(interval);
  }, [ggsAddress]);

  return (
    <GGS_API.Provider
      value={{
        ggsConnectionStatus,
        giraffeStatus,
      }}
    >
      {children}
    </GGS_API.Provider>
  );
};
