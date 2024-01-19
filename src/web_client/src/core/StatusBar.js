import styled from "styled-components";
import { useContext } from "react";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import {
  faSatelliteDish,
  faSatellite,
} from "@fortawesome/free-solid-svg-icons";

import { GGS_API } from "../api_interface/ggs_api.js";
import Tooltip from "../components/Tooltip.js";

const StatusCard = styled.div`
  display: grid;
  grid-gap: 10px;
  grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
  grid-auto-rows: 15px;

  user-select: none;

  font-size: ${(props) => props.theme.fonts.status_bar.size};
  font-family: ${(props) => props.theme.fonts.status_bar.family};
  font-weight: ${(props) => props.theme.fonts.status_bar.weight};
  font-style: ${(props) => props.theme.fonts.status_bar.style};

  background: ${(props) => props.theme.status_background};
  color: ${(props) => props.theme.on_surface};
  position: sticky;
  width: 100% - padding: ${(props) => props.theme.status_bar.padding};
  top: 0;
  padding: ${(props) => props.theme.status_bar.padding};
  min-height: 50px;
  border-bottom-left-radius: ${(props) => props.theme.status_bar.border_radius};
  border-bottom-right-radius: ${(props) =>
    props.theme.status_bar.border_radius};
  z-index: 1;
`;

const BarItemStyle = styled.div`
  white-space: nowrap;
`;
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
      <StatusItem
        title="GDL"
        status={
          ggsConnectionStatus && giraffeStatus.gdl
            ? giraffeStatus.gdl.toUpperCase()
            : "UNKNOWN"
        }
      />
      <StatusItem
        title="GFS"
        status={
          ggsConnectionStatus && giraffeStatus.gfs
            ? giraffeStatus.gfs.toUpperCase()
            : "UNKNOWN"
        }
      />
      <Tooltip text="telemetry up-link status" vertical_position={"-440%"}>
        <StatusItem
          title={<FontAwesomeIcon icon={faSatelliteDish} />}
          status={
            ggsConnectionStatus && giraffeStatus.telemetry_uplink
              ? giraffeStatus.telemetry_uplink.toUpperCase()
              : "UNKNOWN"
          }
        />
      </Tooltip>

      <Tooltip text="telemetry down-link status" vertical_position={"-440%"}>
        <StatusItem
          title={<FontAwesomeIcon icon={faSatellite} />}
          status={
            ggsConnectionStatus && giraffeStatus.telemetry_downlink
              ? giraffeStatus.telemetry_downlink.toUpperCase()
              : "UNKNOWN"
          }
        />
      </Tooltip>

      {/* <StatusItem
        title="InfluxDB"
        status={
          ggsConnectionStatus && giraffeStatus.influxdb
            ? giraffeStatus.influxdb.toUpperCase()
            : "UNKNOWN"
        }
      /> */}
      {/* <StatusItem
        title="aprs.fi"
        status={
          ggsConnectionStatus && giraffeStatus.aprsfi
            ? giraffeStatus.aprsfi.toUpperCase()
            : "UNKNOWN"
        }
      /> */}
    </StatusCard>
  );
}
export default StatusBar;
