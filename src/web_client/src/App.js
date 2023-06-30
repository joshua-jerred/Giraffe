import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom';

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
import GfsMonitorPage from './pages/gfs/data_monitor';
import ConfigureGfsPage from './pages/gfs/configure_gfs';

import ClientSetupPage from './pages/client_setup';
import DocsPage from './pages/docs/docs';
import TestPage from './pages/test';

import NotFound from './pages/404';

function App() {
  return (
    <AppStyle>
      <BrowserRouter>
        <Routes>
          <Route path="/" element={<Layout />}>
          <Route path="/test" element={<TestPage />} />
            <Route index element={<CommandCenter />} />
            <Route path="telemetry" element={<TelemetryPage />} />
            <Route path="tracking" element={<TrackingPage />} />
            <Route path="data-logs" element={<DataLogsPage />} />
            <Route path="configure" element={<ConfigureGgsPage />} />

            <Route
              exact
              path="gfs"
              element={<Navigate to="/gfs/data-monitor" />}
            />
            
            <Route path="gfs">
              <Route path="data-monitor" element={<GfsMonitorPage />} />
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
