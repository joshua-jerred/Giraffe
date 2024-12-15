import styled from "styled-components";

import Tooltip from "../../components/Tooltip";

const StatusStyle = styled.div`
  display: flex;
  justify-content: center;
  align-items: center;

  padding: 0.1em 0.4em;

  background: ${(props) => {
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

const ChipLabel = styled.div``;

const ChipValue = styled.div``;

const ChipStyle = styled.div`
  // border: 2px solid green;

  display: grid;
  grid-template-columns: 2fr 4fr;
  width: fit-content;

  background: ${(props) => props.theme.surface};

  > div {
    border: 1px solid red;
    color: red;
    // margin: "auto 0.1em";
    fontsize: "0.8em";
  }

  :nth-child(1) {
    border-top-left-radius: 0.5em;
    border-bottom-left-radius: 0.5em;
    border: 1px solid ${(props) => props.theme.primary};
  }

  :nth-child(2) {
    border-top-left-radius: 0.5em;
    border-bottom-left-radius: 0.5em;
    // border: 1px solid ${(props) => props.theme.error};
  }
`;

function StatusChip({
  abbreviation,
  status,
  onClick = () => {},
  tooltip = "",
}) {
  return (
    <ChipStyle>
      {/* <Tooltip text={tooltip} vertical_position={"-400%"}> */}
      {/* <StatusStyle status={status}>{abbreviation.toUpperCase()}</StatusStyle> */}
      <div>{abbreviation.toUpperCase()}</div>
      {/* </Tooltip> */}
      <div>{status.toUpperCase()}</div>
    </ChipStyle>
  );
}

export default StatusChip;
