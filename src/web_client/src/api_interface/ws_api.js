import { useState, createContext, useContext, useEffect } from "react";
import useWebSocket, { ReadyState } from "react-use-websocket";
import { GwsGlobal } from "../GlobalContext";
import { parse, StreamRequest } from "giraffe-protocol";

export const GGS_WS = createContext("");

export const GgsWsContextProvider = ({ children }) => {
  const { ggsAddress, clientName } = useContext(GwsGlobal);

  const [statusMessage, setStatusMessage] = useState({
    current_clients: "unknown",
    connection_status: "disconnected",
    telemetry: "unknown",
    gfs: "unknown",
  });

  const onConnect = () => console.log("ws connected");
  const onClosed = () => console.log("ws closed");
  const onError = () => console.log("ws error");
  const onReconnectStop = () => console.log("ws reconnect stop");

  const {
    sendMessage,
    sendJsonMessage,
    lastMessage,
    lastJsonMessage,
    readyState,
  } = useWebSocket("ws://" + ggsAddress + "/api/ws", {
    share: true,
    shouldReconnect: (closeEvent) => true,
    queryParams: { user_name: clientName },
    reconnectAttempts: 10,
    reconnectInterval: 3000,
    onOpen: onConnect,
    onClose: onClosed,
    onError: onError,
    onReconnectStop: onReconnectStop,
  });

  const ggsConnectionStatus = {
    [ReadyState.CONNECTING]: "connecting",
    [ReadyState.OPEN]: "connected",
    [ReadyState.CLOSING]: "closing",
    [ReadyState.CLOSED]: "closed",
    [ReadyState.UNINSTANTIATED]: "uninstantiated",
  }[readyState];

  // Message Received
  useEffect(() => {
    try {
      let received = parse(lastJsonMessage);
      if (received.bdy.cde === "ok" && received.bdy.stream === "status") {
        setStatusMessage(received.body);
      }
      //console.log(received.toString());
    } catch (e) {
      console.log("invalid ws message received");
    }
  }, [lastJsonMessage]);

  // Send Message
  const sendWsMessage = (message) => {
    console.log("Sending Message: " + message);
    if (ggsConnectionStatus === "connected") {
      sendJsonMessage(message.json);
    } else {
      console.log("Not connected, not sending message. " + ggsConnectionStatus);
    }
  };

  const removeAllStreams = () => {
    if (ggsConnectionStatus === "connected") {
      const message = new StreamRequest("gwc", "ggs", "remove_all", "");
      sendWsMessage(message);
    }
  };

  const sendStreamRequest = (stream_name) => {
    const message = new StreamRequest("gwc", "ggs", "add", stream_name);
    sendWsMessage(message);
  };

  return (
    <GGS_WS.Provider
      value={{
        statusMessage,
        sendWsMessage,
        removeAllStreams,
        ggsConnectionStatus,
        sendStreamRequest,
        lastJsonMessage,
      }}
    >
      {children}
    </GGS_WS.Provider>
  );
};
