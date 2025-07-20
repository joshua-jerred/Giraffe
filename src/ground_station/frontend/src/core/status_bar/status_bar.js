import styled, { useTheme } from "styled-components";
import { useContext, useState, useEffect } from "react";
import {
  faSatelliteDish,
  faSatellite,
  faTriangleExclamation,
} from "@fortawesome/free-solid-svg-icons";
import Masonry, { ResponsiveMasonry } from "react-responsive-masonry";

import { GwsGlobal } from "../../GlobalContext.js";

import Tooltip from "../../components/Tooltip.js";

import { useStorageState } from "../LocalStorageState.js";
import { useMissionClockData } from "../../api_interface/mission_clock_api.js";
import { useCurrentUtcTime } from "../../core/clock_strings.js";

// local
import ServiceStatusDisplay from "./service_status_display.js";
import { AlertBar, AlerterToggle } from "./alert_bar.js";

const StatusCard = styled.div`
  display: grid;
  grid-gap: 10px;
  // grid-auto-rows: 15px;
  grid-template-columns: 1fr 1fr 1fr;
  @media(max-width: 800px) {
    grid-template-columns: 1fr 1fr;
  }

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
  grid-template-columns: repeat(1, minmax(200px, 1fr));
  // grid-auto-rows: 15px;
`;

const BarItemStyle = styled.div`
  white-space: nowrap;
  font-size: 15px;

  display: flex;
  justify-content: space-between;
  align-items: center;
  max-width: 18rem;
`;

const BarItemStatusStyle = styled.span`
  color: ${(props) => {
    if (props.noColor) {
      return props.theme.on_surface;
    }

    props.status = props.status.toUpperCase();
    if (props.status === "CONNECTED") {
      return props.theme.success;
    } else if (
      props.status === "DISCONNECTED" ||
      props.status === "ERROR" ||
      props.status === "DOWN"
    ) {
      return props.theme.error;
    } else if (props.status === "UNKNOWN" || props.status === "N/D") {
      return props.theme.warning;
    } else if (props.status === "DISABLED") {
      return props.theme.surface_hover_hard;
    } else {
      return props.theme.primary;
    }
  }};
`;

function StatusItem({
  title,
  tooltip,
  status,
  secondStatus,
  noColor = false,
  onClick,
}) {
  return (
    <Tooltip text={tooltip} vertical_position={"0%"} horizontal_position="60%">
      <BarItemStyle onClick={onClick}>
        {title}
        <BarItemStatusStyle status={status} noColor={noColor}>
          {status}
        </BarItemStatusStyle>
      </BarItemStyle>
    </Tooltip>
  );
}

const StyledTime = styled.div`
  display: flex;
  justify-content: space-between;
  align-items: center;
  max-width: 10rem;
`;

function StatusBar() {
  const {
    serviceStatuses,
    isGgsConnected,
    flightData,
    showVerboseClock,
    setShowVerboseClock,
  } = useContext(GwsGlobal);
  const [expanded, setExpanded] = useState(false);
  const { formattedTime } = useMissionClockData();
  const { currentUtcTime } = useCurrentUtcTime();

  return (
    <>
      <StatusCard>
        <StatusGrid>
          <StatusItem
            title="PHASE"
            tooltip={"The current flight phase"}
            status={
              isGgsConnected && flightData.flight_phase
                ? flightData.flight_phase.toUpperCase()
                : "n/d"
            }
          />
          <StatusItem
            title="Z/MIS"
            tooltip={"Mission Clock"}
            status={currentUtcTime + "Z " + formattedTime}
            noColor={true}
            onClick={() => {
              setShowVerboseClock(!showVerboseClock);
            }}
          />
          {/* <StatusItem
            title="UTC"
            tooltip={"The current UTC time"}
            status={currentUtcTime}
            noColor={true}
            onClick={() => {
              setShowVerboseClock(!showVerboseClock);
            }}
          /> */}
          <StatusItem
            title="ALT V/S"
            tooltip={"The current altitude"}
            status={
              flightData.current_position &&
              (typeof flightData.current_position.altitude === "number"
                ? `${flightData.current_position.altitude} m`
                : "n/d") +
                " " +
                (flightData.current_position &&
                typeof flightData.current_position.vertical_speed === "number"
                  ? `${flightData.current_position.vertical_speed} m/s`
                  : "n/d")
            }
            noColor={true}
          />
          <StatusItem
            title="HDG H/S"
            tooltip={"The current horizontal speed"}
            status={
              (flightData.current_position &&
              typeof flightData.current_position.heading === "number"
                ? `${flightData.current_position.heading}°`
                : "n/d") +
              " " +
              (flightData.current_position &&
              typeof flightData.current_position.horizontal_speed === "number"
                ? `${flightData.current_position.horizontal_speed} m/s`
                : "n/d")
            }
            noColor={true}
          />
        </StatusGrid>
        <ServiceStatusDisplay />
        <AlerterToggle setExpanded={setExpanded} expanded={expanded} />
      </StatusCard>
      <AlertBar expanded={expanded} />
    </>
  );
}
export default StatusBar;
