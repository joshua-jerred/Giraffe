import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom';
import { useEffect, useContext } from 'react';

import { GwsGlobal } from './GlobalContext';
import { AppStyle } from './style/theme';
import Layout from './core/Layout';

// Pages ----------
// GGS
import CommandCenter from './pages/command_center';
import TelemetryPage from './pages/telemetry';
import TrackingPage from './pages/tracking';
import DataLogsPage from './pages/data_logs';
import ConfigureGgsPage from './pages/configure_ggs';
// GFS
import GfsDataStreamPage from './pages/gfs/data_stream';
import ConfigureGfsPage from './pages/gfs/configure_gfs';

import ClientSetupPage from './pages/client_setup';
import DocsPage from './pages/docs/docs';

import NotFound from './pages/404';

function App() {
  const { darkMode, GwsConnectionStatus } = useContext(GwsGlobal);

  // Test connection to the GWS server, and set the connection status.
  function Connect() {}

  // Connect on first load
  if (GwsConnectionStatus === 'unknown') {
    Connect();
  }

  useEffect(() => {
    localStorage.setItem('darkmode', darkMode);
  }, [darkMode]);
  return (
    <AppStyle darkMode={darkMode}>
      <BrowserRouter>
        <Routes>
          <Route path="/" element={<Layout />}>
            <Route index element={<CommandCenter />} />
            <Route path="telemetry" element={<TelemetryPage />} />
            <Route path="tracking" element={<TrackingPage />} />
            <Route path="data-logs" element={<DataLogsPage />} />
            <Route path="configure" element={<ConfigureGgsPage />} />

            <Route
              exact
              path="gfs"
              element={<Navigate to="/gfs/data-stream" />}
            />
            <Route path="gfs">
              <Route path="data-stream" element={<GfsDataStreamPage />} />
              <Route path="configure" element={<ConfigureGfsPage />} />
            </Route>

            <Route path="client-setup" element={<ClientSetupPage />} />
            <Route path="docs" element={<DocsPage />} />

            <Route path="*" element={<NotFound />} />
          </Route>
        </Routes>
      </BrowserRouter>
    </AppStyle>
  );
}

export default App;