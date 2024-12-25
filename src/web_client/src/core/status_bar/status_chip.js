import { useState, useEffect } from "react";
import styled, { useTheme } from "styled-components";

import Tooltip from "../../components/Tooltip";

const StatusStyle = styled.div`
  display: flex;
  justify-content: center;
  align-items: center;

  padding: 0.1em 0.4em;

  color: ${(props) => {
    props.status = props.status.toUpperCase();
    if (props.status === "CONNECTED") {
      return props.theme.on_success;
    } else if (
      props.status === "DISCONNECTED" ||
      props.status === "ERROR" ||
      props.status === "DOWN"
    ) {
      return props.theme.on_error;
    } else if (props.status === "UNKNOWN" || props.status === "N/D") {
      return props.theme.on_warning;
    } else if (props.status === "DISABLED") {
      return props.theme.surface;
    } else {
      return props.theme.on_primary;
    }
  }};
`;

const ChipLabel = styled.div`
  background: ${(props) => props.theme.surface_hover_soft};
  border-radius: 0.5em;
  padding: 0em 0.2em;
  fontsize: 0.8em;
`;

const ChipValue = styled.div`
  padding: 0em 0.2em;
  // fontsize: 0.6em;
`;

const ChipStyle = styled.div`
  display: grid;
  grid-template-columns: 2fr 4fr;
  width: fit-content;

  background: ${(props) => props.theme.surface};
  border: 0.05em solid ${(props) => props.theme.surface_hover_hard};
  border-radius: 0.5em;
`;

function StatusChip({
  abbreviation,
  status,
  onClick = () => {},
  tooltip = "",
}) {
  const theme = useTheme();
  const [statusText, setStatusText] = useState("n/d");
  const [backgroundColor, setBackgroundColor] = useState(theme.surface);
  const [textColor, setTextColor] = useState(theme.on_surface);

  useEffect(() => {
    console.log("StatusChip rendered", status);

    let new_status = status.toUpperCase();
    let new_background = "";
    if (new_status === "CONNECTED") {
      new_background = theme.success;
    } else if (
      new_status === "DISCONNECTED" ||
      new_status === "ERROR" ||
      new_status === "DOWN"
    ) {
      new_background = theme.error;
    } else if (new_status === "UNKNOWN" || new_status === "N/D") {
      new_background = theme.warning;
    } else if (new_status === "DISABLED") {
      new_background = theme.surface_hover_hard;
    } else {
      new_background = theme.primary;
    }

    setStatusText(new_status);
    setBackgroundColor(new_background);
  }, [status]);

  return (
    <ChipStyle>
      {/* <Tooltip text={tooltip} vertical_position={"-400%"}> */}
      {/* <StatusStyle status={status}>{abbreviation.toUpperCase()}</StatusStyle> */}
      <ChipLabel
        style={{
          backgroundColor: backgroundColor,
          color: textColor,
        }}
      >
        {abbreviation.toUpperCase()}
      </ChipLabel>
      {/* </Tooltip> */}
      <ChipValue>{status.toUpperCase()}</ChipValue>
    </ChipStyle>
  );
}

export default StatusChip;
