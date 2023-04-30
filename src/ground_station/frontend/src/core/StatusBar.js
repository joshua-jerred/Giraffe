import styled from 'styled-components';
import { useContext } from 'react';
import { GGS_WS } from '../api_interface/ws_api';

const StatusCard = styled.div`
  display: inline-grid;
  grid-template-columns: auto auto auto;

  font-size: ${(props) => props.theme.fonts.status_bar.size};
  font-family: ${(props) => props.theme.fonts.status_bar.family};
  font-weight: ${(props) => props.theme.fonts.status_bar.weight};
  font-style: ${(props) => props.theme.fonts.status_bar.style};

  background: ${(props) => props.theme.status_background};
  color: ${(props) => props.theme.on_surface};
  position: sticky;
  width: 98%;
  top: 0;
  padding: ${(props) => props.theme.status_bar.padding};
  height: ${(props) => props.theme.status_bar.height};
  border-bottom-left-radius: ${(props) => props.theme.status_bar.border_radius};
  border-bottom-right-radius: ${(props) =>
    props.theme.status_bar.border_radius};
  z-index: 1;
`;

const StatusItem = styled.div``;

function StatusBar() {
  const { statusMessage, ggsConnectionStatus } = useContext(GGS_WS);

  return (
    <StatusCard>
      <StatusItem>GGS Connection: {ggsConnectionStatus}</StatusItem>
      <StatusItem>Telemetry: {statusMessage !== undefined ? statusMessage.telemetry : "unknown"}</StatusItem>
      <StatusItem>GFS: {statusMessage !== undefined ? statusMessage.gfs : "unknown"}</StatusItem>
    </StatusCard>
  );
}
export default StatusBar;
