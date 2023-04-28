import React from 'react';
import useWebSocket, { ReadyState } from 'react-use-websocket';

function loadTheme() {
  const theme = localStorage.getItem('darkmode');
  if (theme !== 'false') {
    return true;
  }
  return false;
}

export const GwsGlobal = React.createContext(null);

export const GwsGlobalContextProvider = ({ children }) => {
  const [darkMode, setDarkMode] = React.useState(loadTheme());

  const [ggsSocketAddress, setGgsSocketAddress] = React.useState(() => {
    const gws_server_address =
      window.localStorage.getItem('ggs_server_address');
    return gws_server_address || '';
  });
  React.useEffect(() => {
    window.localStorage.setItem(
      'ggs_server_address',
      ggsSocketAddress !== null ? ggsSocketAddress : ''
    );
  }, [ggsSocketAddress]);

  const {
    sendMessage,
    sendJsonMessage,
    lastMessage,
    lastJsonMessage,
    readyState,
    getWebSocket,
  } = useWebSocket('ws://' + ggsSocketAddress, {
    share: true,
    shouldReconnect: (closeEvent) => true,
    reconnectAttempts: 10,
    reconnectInterval: 3000,
  });

  const ggsConnectionStatus = {
    [ReadyState.CONNECTING]: 'connecting',
    [ReadyState.OPEN]: 'connected',
    [ReadyState.CLOSING]: 'closing',
    [ReadyState.CLOSED]: 'closed',
    [ReadyState.UNINSTANTIATED]: 'uninstantiated',
  }[readyState];

  return (
    <GwsGlobal.Provider
      value={{
        darkMode,
        setDarkMode,
        ggsSocketAddress,
        setGgsSocketAddress,
        sendMessage,
        sendJsonMessage,
        lastMessage,
        lastJsonMessage,
        readyState,
        getWebSocket,
        ggsConnectionStatus,
      }}
    >
      {children}
    </GwsGlobal.Provider>
  );
};
