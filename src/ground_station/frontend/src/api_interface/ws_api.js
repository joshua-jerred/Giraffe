import { useState, createContext, useContext, useEffect } from 'react';
import useWebSocket, { ReadyState } from 'react-use-websocket';
import { GwsGlobal } from '../GlobalContext';
import { parse, PathMessage, StreamRequest } from 'giraffe-protocol';

export const GGS_WS = createContext("");

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
  const onError = () => console.log('ws error');
  const onReconnectStop = () => console.log('ws reconnect stop');

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
    onError: onError,
    onReconnectStop: onReconnectStop,
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
    //console.log('Message Received: ' + lastJsonMessage);
    setNumMessages(numMessages + 1);
    try {
      let received = parse(lastJsonMessage);
      if (received.type === "status") {
        setStatusMessage(received.body);
      }
      //console.log(received.toString());
    } catch (e) {
      console.log("invalid ws message received");
    }

  }, [lastJsonMessage]);

  // Send Message
  const sendWsMessage = (message) => {
    console.log('Sending Message: ' + message);
    if (ggsConnectionStatus === 'connected') {
      sendJsonMessage(message.json);
    }
  };

  const sendPathMessage = (path) => {
    const message = new PathMessage(path);
    sendWsMessage(message);
  }

  const sendStreamRequest = (stream_name) => {
    const message = new StreamRequest('client', 'ggs', 'add', stream_name);
    sendWsMessage(message);
  }

  return (
    <GGS_WS.Provider
      value={{
        statusMessage,
        sendWsMessage,
        sendPathMessage,
        ggsConnectionStatus,
        sendStreamRequest,
        lastJsonMessage
      }}
    >
      {children}
    </GGS_WS.Provider>
  );
};



