import styled from "styled-components";
import { useContext, useState, useEffect } from "react";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import {
  faSatelliteDish,
  faSatellite,
} from "@fortawesome/free-solid-svg-icons";

import { GwsGlobal } from "../GlobalContext";
import { NavLink } from "react-router-dom";

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

const AlertBarStyle = styled.div`
display: flex;
justify-content: center;
align-items: center;

background: linear-gradient(${(props) => props.theme.status_background}, ${(
  props
) => props.theme.alert_bar_background});
color: ${(props) => props.theme.on_error};
position: sticky;
width: 100% - padding: ${(props) => props.theme.status_bar.padding};
top: 0;
margin: 0px 30px;
height: ${(props) => (props.expanded ? "fit-content" : "0px")};
border-bottom-left-radius: ${(props) => props.theme.status_bar.border_radius};
border-bottom-right-radius: ${(props) => props.theme.status_bar.border_radius};
overflow: ${(props) => (props.expanded ? "visible" : "hidden")};
`;

const AlertBarContentStyle = styled.ul`
  width: fill-available;

  & > li {
    font-size: 0.9rem;
    font-family: ${(props) => props.theme.fonts.status_bar.family};
    font-weight: 400;
    font-style: ${(props) => props.theme.fonts.status_bar.style};
  }
  & > li > a {
    color: ${(props) => props.theme.on_error};
  }
`;

function AlertBar() {
  const { alerter } = useContext(GwsGlobal);
  const [expanded, setExpanded] = useState(false);

  const CHECK_CONNECTION_INTERVAL = 1000;

  useEffect(() => {
    const updateAlerts = () => {
      if (alerter.getNumAlerts() > 0) {
        setExpanded(true);
      } else {
        setExpanded(false);
      }
    };

    const interval = setInterval(() => {
      updateAlerts();
    }, CHECK_CONNECTION_INTERVAL);
    updateAlerts();
    return () => clearInterval(interval);
  }, [alerter]);

  return (
    <AlertBarStyle expanded={expanded}>
      {" "}
      <AlertBarContentStyle>
        {Object.keys(alerter.getAlerts()).map((alert_id) => {
          return (
            // conditionally render the item as a link if one was provided
            <li key={alert_id}>
              {alerter.getAlert(alert_id).fix_link !== null ? (
                <NavLink to={alerter.getAlert(alert_id).fix_link}>
                  {alerter.getAlert(alert_id).text}
                </NavLink>
              ) : (
                alerter.getAlert(alert_id).text
              )}
            </li>
          );
        })}
      </AlertBarContentStyle>
    </AlertBarStyle>
  );
}

function StatusBar() {
  const { serviceStatuses, isGgsConnected } = useContext(GwsGlobal);

  return (
    <>
      <StatusCard>
        <StatusItem
          title="GGS"
          status={isGgsConnected ? "CONNECTED" : "DISCONNECTED"}
        />
        <StatusItem
          title="GDL"
          status={
            isGgsConnected && serviceStatuses.gdl
              ? serviceStatuses.gdl.toUpperCase()
              : "UNKNOWN"
          }
        />
        <StatusItem
          title="GFS"
          status={
            isGgsConnected && serviceStatuses.gfs
              ? serviceStatuses.gfs.toUpperCase()
              : "UNKNOWN"
          }
        />
        <Tooltip text="telemetry up-link status" vertical_position={"-440%"}>
          <StatusItem
            title={<FontAwesomeIcon icon={faSatelliteDish} />}
            status={
              isGgsConnected && serviceStatuses.telemetry_uplink
                ? serviceStatuses.telemetry_uplink.toUpperCase()
                : "UNKNOWN"
            }
          />
        </Tooltip>

        <Tooltip text="telemetry down-link status" vertical_position={"-440%"}>
          <StatusItem
            title={<FontAwesomeIcon icon={faSatellite} />}
            status={
              isGgsConnected && serviceStatuses.telemetry_downlink
                ? serviceStatuses.telemetry_downlink.toUpperCase()
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
      <AlertBar />
    </>
  );
}
export default StatusBar;
