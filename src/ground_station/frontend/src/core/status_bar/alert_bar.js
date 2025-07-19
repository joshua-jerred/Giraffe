import { useContext, useState, useEffect } from "react";
import styled, { useTheme } from "styled-components";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { GwsGlobal } from "../../GlobalContext.js";
import { NavLink } from "react-router-dom";
import Tooltip from "../../components/Tooltip.js";
import { faTriangleExclamation } from "@fortawesome/free-solid-svg-icons";

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
z-index: 1000;
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
function Alert({ alert_id, alert_text, fix_link, suppressed }) {
  const { alerter } = useContext(GwsGlobal);
  const [isSuppressed, setIsSuppressed] = useState(suppressed);
  const [suppressIcon, setSuppressIcon] = useState(
    isSuppressed ? "fa-bell" : "fa-bell-slash"
  );

  useEffect(() => {
    const suppressed = alerter.getAlert(alert_id).suppressed;
    setIsSuppressed(suppressed);
    setSuppressIcon(suppressed ? "fa-bell" : "fa-bell-slash");
  }, [alerter, alert_id, isSuppressed, suppressIcon, suppressed]);

  const alertDismissed = (alert_id) => {
    if (alerter.getAlert(alert_id).suppressed) {
      alerter.unSuppressAlert(alert_id);
      setIsSuppressed(alerter.getAlert(alert_id).suppressed);
    } else {
      alerter.suppressAlert(alert_id);
      setIsSuppressed(alerter.getAlert(alert_id).suppressed);
    }
  };

  return (
    <AlertStyle
      style={
        {
          // display: alertDismissed ? "none" : "flex",
        }
      }
    >
      {fix_link !== null ? (
        <NavLink to={fix_link}>{alert_text}</NavLink>
      ) : (
        { alert_text }
      )}
      <Tooltip
        text={isSuppressed ? "Unsuppress Alert" : "Suppress Alert"}
        vertical_position={"100%"}
        flip_horizontal={true}
        specified_delay={500}
        style={{
          display: "flex",
          justifyContent: "center",
          alignItems: "center",
          // flexDirection: "column",
        }}
      >
        {isSuppressed && (
          <span
            style={{
              fontSize: "0.7em",
              margin: "0px 5px",
            }}
          >
            suppressed
          </span>
        )}
        <AlertSuppressIcon
          className={`fa ${suppressIcon}`}
          onClick={() => alertDismissed(alert_id)}
        />
      </Tooltip>
    </AlertStyle>
  );
}

export function AlertBar({ expanded }) {
  const { alerter } = useContext(GwsGlobal);

  const CHECK_CONNECTION_INTERVAL = 1000;

  useEffect(() => {
    const updateAlerts = () => {
      if (alerter.getNumAlerts() > 0) {
        // setExpanded(true);
      } else {
        // setExpanded(false);
      }
    };

    const interval = setInterval(() => {
      updateAlerts();
    }, CHECK_CONNECTION_INTERVAL);
    updateAlerts();
    return () => clearInterval(interval);
  }, [alerter]);

  return (
    <AlertBarStyle expanded={expanded || alerter.getNumAlerts(false) > 0}>
      <AlertBarContentStyle>
        {Object.keys(alerter.getAlerts(expanded)).map((alert_id) => {
          return (
            // conditionally render the item as a link if one was provided
            <Alert
              key={alert_id}
              alert_id={alert_id}
              alert_text={alerter.getAlert(alert_id).text}
              fix_link={alerter.getAlert(alert_id).fix_link}
              suppressed={alerter.getAlert(alert_id).suppressed}
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

export function AlerterToggle({ setExpanded, expanded }) {
  const theme = useTheme();
  const { alerter } = useContext(GwsGlobal);
  const [totalNumActiveAlerts, setTotalNumActiveAlerts] = useState(
    alerter.getNumAlerts(true)
  );
  const [numAlertsNotDismissed, setNumAlertsNotDismissed] = useState(
    alerter.getNumAlerts(false)
  );

  useEffect(() => {
    const updateAlerts = () => {
      setNumAlertsNotDismissed(alerter.getNumAlerts(false));
      setTotalNumActiveAlerts(alerter.getNumAlerts(true));
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
      onClick={() => setExpanded(!expanded)}
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
            color:
              numAlertsNotDismissed > 0
                ? theme.error
                : totalNumActiveAlerts > 0
                ? theme.warning
                : theme.surface_alt,
          }}
        />
        <div
          style={{
            display: "flex",
            fontSize: "0.8em",
            justifyContent: "center",
            alignItems: "center",
            wordWrap: "break-all",
            width: "100%",
            textAlign: "center",
            color:
              numAlertsNotDismissed > 0
                ? theme.error
                : totalNumActiveAlerts > 0
                ? theme.warning
                : theme.surface_alt,
          }}
        >
          {numAlertsNotDismissed}/{totalNumActiveAlerts} Active Alerts
        </div>
      </Tooltip>
    </div>
  );
}
