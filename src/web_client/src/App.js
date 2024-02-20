import { BrowserRouter, Routes, Route, Navigate } from "react-router-dom";

import { AppStyle } from "./style/theme";
import Layout from "./core/Layout";

import "./leaflet/leaflet.css";

import { GwsGlobalContextProvider } from "./GlobalContext";

// Pages ----------
// GGS
import CommandCenter from "./pages/command_center";
import TelemetryPage from "./pages/telemetry";
import DataLogsPage from "./pages/data_logs";
// GFS
import GfsMonitorPage from "./pages/gfs/data_monitor";
import ConfigureGfsPage from "./pages/gfs/configure_gfs";
import SetupPage from "./pages/setup";
import DocsPage from "./pages/docs/docs";
import NotFound from "./pages/404";

function App() {
  return (
    <GwsGlobalContextProvider>
      <AppStyle>
        <BrowserRouter>
          <Routes>
            <Route path="/" element={<Layout />}>
              <Route index element={<CommandCenter />} />
              <Route path="telemetry" element={<TelemetryPage />} />
              <Route path="data-logs" element={<DataLogsPage />} />

              <Route
                exact
                path="gfs"
                element={<Navigate to="/gfs/data-monitor" />}
              />

              <Route path="gfs">
                <Route path="data-monitor" element={<GfsMonitorPage />} />
                <Route path="configure" element={<ConfigureGfsPage />} />
              </Route>

              <Route path="setup" element={<SetupPage />} />
              <Route path="docs" element={<DocsPage />} />

              <Route path="*" element={<NotFound />} />
            </Route>
          </Routes>
        </BrowserRouter>
      </AppStyle>
    </GwsGlobalContextProvider>
  );
}

export default App;
