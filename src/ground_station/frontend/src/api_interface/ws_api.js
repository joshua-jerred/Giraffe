import { useState, createContext, useContext, useEffect } from 'react';
import useWebSocket, { ReadyState } from 'react-use-websocket';
import { GwsGlobal } from '../GlobalContext';
import { PathMessage } from 'giraffe-protocol/socket_schema';
import { parseMessage } from 'giraffe-protocol/socket_schema';

export const GGS_WS = createContext(null);

export const GgsWsContextProvider = ({ children }) => {
  const { ggsAddress, clientName } = useContext(GwsGlobal);

  const [statusMessage, setStatusMessage] = useState({
    current_clients: 'unknown',
    connection_status: 'disconnected',
    telemetry_connection: 'unknown',
    gfs_connection: 'unknown',
  });

  const onConnect = () => console.log('ws connected');
  const onClosed = () => console.log('ws closed');

  const {
    sendMessage,
    sendJsonMessage,
    lastMessage,
    lastJsonMessage,
    readyState,
  } = useWebSocket('ws://' + ggsAddress + '/api/ws', {
    share: true,
    shouldReconnect: (closeEvent) => true,
    queryParams: { client_name: clientName },
    reconnectAttempts: 10,
    reconnectInterval: 3000,
    onOpen: onConnect,
    onClose: onClosed,
  });

  const ggsConnectionStatus = {
    [ReadyState.CONNECTING]: 'connecting',
    [ReadyState.OPEN]: 'connected',
    [ReadyState.CLOSING]: 'closing',
    [ReadyState.CLOSED]: 'closed',
    [ReadyState.UNINSTANTIATED]: 'uninstantiated',
  }[readyState];

  // Message Received
  const [numMessages, setNumMessages] = useState(0);
  useEffect(() => {
    setNumMessages(numMessages + 1);
    try {
      let received = parseMessage(lastJsonMessage);
      if (received.type === "status") {
        setStatusMessage(received.body);
        console.log(received.body);
      }
      //console.log(received.toString());
    } catch (e) {
      console.log("invalid ws message received");
    }

  }, [lastJsonMessage]);

  // Send Message
  const sendWsMessage = (message) => {
    console.log('Sending Message: ' + message.string());
    if (ggsConnectionStatus === 'connected') {
      sendJsonMessage(message.json());
    }
  };

  const sendPathMessage = (path) => {
    const message = new PathMessage(path);
    sendWsMessage(message);
  }

  return (
    <GGS_WS.Provider
      value={{
        statusMessage,
        sendWsMessage,
        sendPathMessage,
        ggsConnectionStatus,
      }}
    >
      {children}
    </GGS_WS.Provider>
  );
};



