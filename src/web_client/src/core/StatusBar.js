import styled from "styled-components";
import { useContext, useState, useEffect } from "react";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import {
  faSatelliteDish,
  faSatellite,
  faTriangleExclamation,
} from "@fortawesome/free-solid-svg-icons";

import { GwsGlobal } from "../GlobalContext";
import { NavLink } from "react-router-dom";

import Tooltip from "../components/Tooltip.js";

import { useStorageState } from "./LocalStorageState.js";

const StatusCard = styled.div`
  display: grid;
  // grid-gap: 10px;
  grid-template-columns: 5fr 1fr;
  // grid-auto-rows: 15px;

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
  z-index: 1001;
`;

const StatusGrid = styled.div`
  display: grid;
  grid-gap: 10px;
  grid-template-columns: repeat(auto-fill, minmax(170px, 1fr));
  grid-auto-rows: 15px;
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
    } else if (props.status === "UNKNOWN") {
      return props.theme.warning;
    } else {
      return props.theme.primary;
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

const AlertBarContentStyle = styled.div`
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;

  width: 95%;
  margin: 0% 0%;
  padding: 1% 0%;
`;

const AlertStyle = styled.div`
  font-size: 0.9rem;
  font-family: ${(props) => props.theme.fonts.status_bar.family};
  font-weight: 400;
  font-style: ${(props) => props.theme.fonts.status_bar.style};

  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 0.8em 0.8em;
  margin: 0.3em 0em;
  // margin: 0.5em 0em;
  width: 95%;
  height: 1.5rem;

  transition: ${(props) => props.theme.transitions.default};

  border-radius: 10px;
  cursor: pointer;
  outline: 1px solid ${(props) => props.theme.surface};

  opacity: 0.9;
  background: ${(props) => props.theme.error_hover}10;
  &:hover {
    opacity: 0.9;
    background: ${(props) => props.theme.error_hover}60;
  }

  &:first-child {
    transition: ${(props) => props.theme.transitions.default};
    &:hover {
      background: linear-gradient(
        ${(props) => props.theme.error_hover}10,
        ${(props) => props.theme.error_hover}10
      );
    }

    // margin-top: 0.5em;
    background: linear-gradient(
      ${(props) => props.theme.status_background}20,
      ${(props) => props.theme.alert_bar_background}20
    );
  }

  & > a {
    color: ${(props) => props.theme.on_error};
  }
`;

const AlertSuppressIcon = styled.i`
  font-size: 1.4em;

  text-align: center;
  vertical-align: middle;
  cursor: pointer;
  transition: ${(props) => props.theme.transitions.default};

  color: ${(props) => props.theme.surface};
  &:hover {
    color: ${(props) => props.theme.surface_hover};
  }
  &:active {
    color: ${(props) => props.theme.primary};
  }
`;
function Alert({ alert_id, alert_text, fix_link }) {
  const SUPPRESS_ICON = "fa-bell-slash";
  const [alertDismissed, setAlertDismissed] = useStorageState(
    `alert_${alert_id}_dismissed`,
    false
  );

  return (
    <AlertStyle
      style={{
        display: alertDismissed ? "none" : "flex",
      }}
    >
      {" "}
      {fix_link !== null ? (
        <NavLink to={fix_link}>{alert_text}</NavLink>
      ) : (
        { alert_text }
      )}
      <Tooltip
        text="Dismiss Alert"
        vertical_position={"100%"}
        flip_horizontal={true}
        specified_delay={500}
      >
        <AlertSuppressIcon
          className={`fa ${SUPPRESS_ICON}`}
          onClick={() => setAlertDismissed(true)}
        />
      </Tooltip>
    </AlertStyle>
  );
}

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
      <AlertBarContentStyle>
        {Object.keys(alerter.getAlerts()).map((alert_id) => {
          return (
            // conditionally render the item as a link if one was provided
            <Alert
              key={alert_id}
              alert_id={alert_id}
              alert_text={alerter.getAlert(alert_id).text}
              fix_link={alerter.getAlert(alert_id).fix_link}
            >
              {alerter.getAlert(alert_id).fix_link !== null ? (
                <NavLink to={alerter.getAlert(alert_id).fix_link}>
                  {alerter.getAlert(alert_id).text}
                </NavLink>
              ) : (
                alerter.getAlert(alert_id).text
              )}
            </Alert>
          );
        })}
      </AlertBarContentStyle>
    </AlertBarStyle>
  );
}

function AlerterToggle({ setExpanded, expanded }) {
  const { alerter } = useContext(GwsGlobal);
  const [numAlerts, setNumAlerts] = useState(alerter.getNumAlerts());

  useEffect(() => {
    const updateAlerts = () => {
      setNumAlerts(alerter.getNumAlerts());
    };

    const interval = setInterval(() => {
      updateAlerts();
    }, 1000);
    updateAlerts();
    return () => clearInterval(interval);
  }, [alerter]);

  return (
    <div
      style={{
        display: "flex",
        flexDirection: "column",
        justifyContent: "center",
        alignItems: "center",
        cursor: "pointer",
      }}
    >
      <Tooltip
        text={`Toggle the full alert list - see dismissed alerts`}
        vertical_position={"-60%"}
        flip_horizontal={true}
        horizontal_position={"100%"}
      >
        <FontAwesomeIcon
          icon={faTriangleExclamation}
          style={{
            display: "flex",
            width: "100%",
            fontSize: "2em",
          }}
        />
        <div
          style={{
            display: "flex",
            fontSize: "0.8em",
            color: "white",
            justifyContent: "center",
            alignItems: "center",
            wordWrap: "break-all",
            width: "100%",
            textAlign: "center",
          }}
        >
          {numAlerts} Active Alerts
        </div>
      </Tooltip>
    </div>
  );
}

function StatusBar() {
  const { serviceStatuses, isGgsConnected, flightData } = useContext(GwsGlobal);
  const [expanded, setExpanded] = useState(false);

  return (
    <>
      <StatusCard>
        <StatusGrid>
          {/* <StatusItem
          title="GGS"
          status={isGgsConnected ? "CONNECTED" : "DISCONNECTED"}
          /> */}
          {/* <StatusItem
          title="GDL"
          status={
            isGgsConnected && serviceStatuses.gdl
            ? serviceStatuses.gdl.toUpperCase()
            : "UNKNOWN"
          }
          /> */}
          <Tooltip text="GFS TCP Connection Status" vertical_position={"-600%"}>
            <StatusItem
              title="GFS"
              status={
                isGgsConnected && serviceStatuses.gfs
                  ? serviceStatuses.gfs.toUpperCase()
                  : "UNKNOWN"
              }
            />
          </Tooltip>
          <Tooltip
            text="Flight System Agent Connection Status"
            vertical_position={"-600%"}
          >
            <StatusItem
              title="FSA"
              status={
                isGgsConnected && serviceStatuses.fsa
                  ? serviceStatuses.fsa.toUpperCase()
                  : "UNKNOWN"
              }
            />
          </Tooltip>
          <Tooltip
            text="The status of the flight software according to the flight system agent."
            vertical_position={"-700%"}
            horizontal_position={"70%"}
          >
            <StatusItem
              title="FSA-GFS"
              status={
                isGgsConnected && serviceStatuses.fsa
                  ? serviceStatuses.fsa_gfs_status.toUpperCase()
                  : "UNKNOWN"
              }
            />
          </Tooltip>
          <Tooltip text="telemetry up-link status" vertical_position={"-600%"}>
            <StatusItem
              title={<FontAwesomeIcon icon={faSatelliteDish} />}
              status={
                isGgsConnected && serviceStatuses.telemetry_uplink
                  ? serviceStatuses.telemetry_uplink.toUpperCase()
                  : "UNKNOWN"
              }
            />
          </Tooltip>

          <Tooltip
            text="telemetry down-link status"
            vertical_position={"-600%"}
          >
            <StatusItem
              title={<FontAwesomeIcon icon={faSatellite} />}
              status={
                isGgsConnected && serviceStatuses.telemetry_downlink
                  ? serviceStatuses.telemetry_downlink.toUpperCase()
                  : "UNKNOWN"
              }
            />
          </Tooltip>
          <Tooltip text="The current flight phase" vertical_position={"-600%"}>
            <StatusItem
              title="PHASE"
              status={
                isGgsConnected && flightData.flight_phase
                  ? flightData.flight_phase.toUpperCase()
                  : "UNKNOWN"
              }
            />
          </Tooltip>
        </StatusGrid>
        <AlerterToggle setExpanded={setExpanded} expanded={expanded} />
      </StatusCard>
      <AlertBar />
    </>
  );
}
export default StatusBar;
