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
  padding: 0em 0.3em;
`;

const ChipValue = styled.div`
  padding: 0em 0.5em;
`;

const ChipStyle = styled.div`
  display: grid;
  grid-template-columns: 2fr 4fr;
  width: min-content;

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

    let new_status_text = status.toUpperCase();
    let new_background = "";
    let new_text_color = "";
    if (new_status_text === "CONNECTED") {
      new_status_text = "OK";
      new_background = theme.success;
      new_text_color = theme.on_success;
    } else if (
      new_status_text === "DISCONNECTED" ||
      new_status_text === "ERROR" ||
      new_status_text === "DOWN"
    ) {
      if (new_status_text === "DISCONNECTED") {
        new_status_text = "DS";
      } else if (new_status_text === "ERROR") {
        new_status_text = "ER";
      }

      new_background = theme.error;
      new_text_color = theme.on_error;
    } else if (new_status_text === "UNKNOWN" || new_status_text === "N/D") {
      if (new_status_text === "UNKNOWN") {
        new_status_text = "UK";
      } else if (new_status_text === "N/D") {
        new_status_text = "N/D";
      }
      new_background = theme.warning;
      new_text_color = theme.on_warning;
    } else if (new_status_text === "DISABLED") {
      new_background = theme.surface_hover_hard;
      new_text_color = theme.on_surface;
    } else {
      new_background = theme.primary;
      new_text_color = theme.on_primary;
    }

    setStatusText(new_status_text);
    setBackgroundColor(new_background);
    setTextColor(new_text_color);
  }, [status]);

  return (
    <Tooltip text={tooltip} vertical_position={"-400%"}>
      <ChipStyle>
        <ChipLabel
          style={{
            backgroundColor: backgroundColor,
            color: textColor,
          }}
        >
          {abbreviation.toUpperCase()}
        </ChipLabel>
        <ChipValue>{statusText}</ChipValue>
      </ChipStyle>
    </Tooltip>
  );
}

export default StatusChip;
