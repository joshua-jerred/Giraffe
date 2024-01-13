import styled from "styled-components";
import { useContext } from "react";
import { GGS_API } from "../api_interface/ws_api";

const StatusCard = styled.div`
  display: inline-grid;
  grid-template-columns: auto auto auto auto;

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

const BarItemStyle = styled.div``;
const BarItemStatusStyle = styled.span`
  color: ${(props) => {
    if (props.status === "CONNECTED") {
      return props.theme.success;
    } else if (props.status === "DISCONNECTED") {
      return props.theme.error;
    } else {
      return props.theme.warning;
    }
  }};
`;
function StatusItem(props) {
  return (
    <BarItemStyle>
      {props.title}:{" "}
      <BarItemStatusStyle status={props.status}>
        {props.status}
      </BarItemStatusStyle>
    </BarItemStyle>
  );
}

function StatusBar() {
  const { giraffeStatus, ggsConnectionStatus } = useContext(GGS_API);

  return (
    <StatusCard>
      <StatusItem
        title="GGS"
        status={ggsConnectionStatus ? "CONNECTED" : "DISCONNECTED"}
      />
      <StatusItem title="GFS" status={giraffeStatus.gfs.toUpperCase()} />
      <StatusItem
        title="GDL"
        status={
          ggsConnectionStatus && giraffeStatus.gdl
            ? giraffeStatus.gdl.toUpperCase()
            : "UNKNOWN"
        }
      />
      <StatusItem
        title="Telemetry"
        status={
          ggsConnectionStatus && giraffeStatus.telemetry
            ? giraffeStatus.telemetry.toUpperCase()
            : "UNKNOWN"
        }
      />
      <StatusItem
        title="InfluxDB"
        status={
          ggsConnectionStatus && giraffeStatus.influxdb
            ? giraffeStatus.influxdb.toUpperCase()
            : "UNKNOWN"
        }
      />
      <StatusItem
        title="aprs.fi"
        status={
          ggsConnectionStatus && giraffeStatus.aprsfi
            ? giraffeStatus.aprsfi.toUpperCase()
            : "UNKNOWN"
        }
      />
    </StatusCard>
  );
}
export default StatusBar;
