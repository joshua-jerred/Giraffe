import { BrowserRouter, Routes, Route, Navigate } from "react-router-dom";

import { AppStyle } from "./style/theme";
import Layout from "./core/Layout";

import "./leaflet/leaflet.css";

import { GwsGlobalContextProvider } from "./GlobalContext";

// Pages ----------
import NotFound from "./pages/404";
import DocsPage from "./pages/docs/docs";
import SetupPage from "./pages/setup";
// GGS
import CommandCenter from "./pages/command_center";
import TelemetryPage from "./pages/telemetry";
import DataLogsPage from "./pages/data_logs";
import DiagnosticsPage from "./core/diagnostics_page";
// GFS
import GfsMonitorPage from "./pages/gfs/data_monitor";
import ConfigureGfsPage from "./pages/gfs/configure_gfs";
import ImageryPage from "./pages/gfs/imagery";

function App() {
  return (
    <GwsGlobalContextProvider>
      <AppStyle>
        <BrowserRouter>
          <Routes>
            <Route path="/" element={<Layout />}>
              <Route index element={<CommandCenter />} />

              <Route
                exact
                path="ggs"
                element={<Navigate to="/ggs/command-center" />}
              />
              <Route path="ggs">
                <Route path="command-center" element={<CommandCenter />} />
                <Route path="telemetry" element={<TelemetryPage />} />
                <Route path="data-logs" element={<DataLogsPage />} />
                <Route path="diagnostics" element={<DiagnosticsPage />} />
              </Route>

              <Route
                exact
                path="gfs"
                element={<Navigate to="/gfs/data-monitor" />}
              />
              <Route path="gfs">
                <Route path="data-monitor" element={<GfsMonitorPage />} />
                <Route path="configure" element={<ConfigureGfsPage />} />
                <Route path="imagery" element={<ImageryPage />} />
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
