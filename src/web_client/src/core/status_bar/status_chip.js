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

const ChipStyle = styled.div`
  border: 2px solid green;
  background: ${(props) => props.theme.surface};
  display: grid;
  grid-template-columns: 2fr 4fr;
  width: fit-content;

  &:nth-child(1) {
    border-top-left-radius: 0.5em;
    border-bottom-left-radius: 0.5em;
    border: 1px solid ${(props) => props.theme.primary};
  }

  &:nth-child(2) {
    border-top-left-radius: 0.5em;
    border-bottom-left-radius: 0.5em;
    border: 1px solid ${(props) => props.theme.error};
  }
`;

function StatusChip({ abbreviation, status, onClick = () => {} }) {
  return (
    // <Tooltip text={title} vertical_position={"-600%"}>
    <ChipStyle>
      <StatusStyle status={status}>{abbreviation}</StatusStyle>
      <div
        style={{
          width: "100%",
          height: "100%",

          display: "flex",
          // justifyContent: "center",
          alignItems: "center",
          // padding: "1em 0.25em",
          margin: "auto 0.1em",
          fontSize: "0.6em",
        }}
      >
        {status}
      </div>
    </ChipStyle>
    // </Tooltip>
  );
}

export default StatusChip;
