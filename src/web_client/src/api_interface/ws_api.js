import { useState, createContext, useContext, useEffect } from "react";
import useWebSocket, { ReadyState } from "react-use-websocket";
import { GwsGlobal } from "../GlobalContext";
import { parse, StreamRequest } from "giraffe-protocol";

export const GGS_API = createContext("");

export const GgsApiContextProvider = ({ children }) => {
  const { ggsAddress, clientName } = useContext(GwsGlobal);

  const [ggsConnectionStatus, setGgsConnectionStatus] = useState(false);

  const [giraffeStatus, setGiraffeStatus] = useState({
    current_clients: "unknown",
    connection_status: "disconnected",
    telemetry: "unknown",
    gfs: "unknown",
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
