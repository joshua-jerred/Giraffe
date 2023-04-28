import styled from 'styled-components';
import { useContext, useEffect, useState } from 'react';
import { GwsGlobal } from '../GlobalContext';

const StatusCard = styled.div`
  display: inline-grid;
  grid-template-columns: auto auto;

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
  const { ggsConnectionStatus, lastJsonMessage } = useContext(GwsGlobal);
  const [current_clients, setCurrentClients] = useState(0);
  const [telemetry_connection, setTelemetryConnection] =
    useState('disconnected');
  const [gfs_connection, setGfsConnection] = useState('disconnected');

  useEffect(() => {
    if (lastJsonMessage !== null && lastJsonMessage['type'] === 'status') {
      let body = lastJsonMessage['body'];
      setCurrentClients(body['num_clients']);
      setTelemetryConnection(body['telemetry_connection']);
      setGfsConnection(body['gfs_connection']);
    }
  }, [lastJsonMessage]);

  return (
    <StatusCard>
      <StatusItem>GGS Connection: {ggsConnectionStatus}</StatusItem>
      <StatusItem>Current Clients: {current_clients}</StatusItem>
      <StatusItem>Telemetry: {telemetry_connection}</StatusItem>
      <StatusItem>GFS: {gfs_connection}</StatusItem>
    </StatusCard>
  );
}
export default StatusBar;
