import React from 'react';

export const GwsGlobal = React.createContext("");
export const GwsGlobalContextProvider = ({ children }) => {
  function save(key, value) {
    window.localStorage.setItem(key, value);
  }
  function load(key) {
    return window.localStorage.getItem(key);
  }

  const [clientDarkTheme, setClientDarkTheme] = React.useState(load('client_dark_theme') !== 'false');
  const [clientName, setClientName] = React.useState(load('client_name') || 'not set');
  const [ggsAddress, setGgsAddress] = React.useState(load('client_ggs_address') || 'http://localhost:7892');

  React.useEffect(() => {
    save('client_dark_theme', clientDarkTheme);
    save('client_name', clientName);
    save('client_ggs_address', ggsAddress);
  }, [clientDarkTheme, clientName, ggsAddress]);

  return (
    <GwsGlobal.Provider
      value={{
        clientDarkTheme,
        clientName,
        ggsAddress,
        setClientDarkTheme,
        setClientName,
        setGgsAddress,
      }}
    >
      {children}
    </GwsGlobal.Provider>
  );
};
