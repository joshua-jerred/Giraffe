import React from "react";
import styled from "styled-components";
import Tooltip from "../../../components/Tooltip.js";

const MapMenuBarStyle = styled.div`
  display: flex;
  justify-content: space-around;
  align-items: center;
  background-color: ${(props) => props.theme.surface};
  padding: 0.5em;
  padding-top: 0em;
`;

const MapMenuButtonStyle = styled.div`
  width: 1.5em;
  height: 1.5em;
  border: 1px solid ${(props) => props.theme.surface_alt_2};
  border-radius: 5px;
  background-color: ${(props) => props.theme.surface_alt};

  font-size: 1.3em;
  font-weight: 600;

  display: flex;
  flex-direction: row;
  justify-content: center;
  align-items: center;

  &:hover {
    opacity: 0.8;
    cursor: pointer;
  }
  &:active {
    background-color: ${(props) =>
      props.disabled ? "" : props.theme.surface_alt_2};
    transform: ${(props) => (props.disabled ? "" : "scale(0.95)")};
  }
`;

function MapMenuButton({
  label_letter,
  onClick,
  tooltip_text,
  active = true,
  disabled = false,
}) {
  return (
    <Tooltip
      text={tooltip_text}
      style={{ zIndex: 1000 }}
      horizontal_position="10%"
    >
      <MapMenuButtonStyle
        onClick={() => {
          if (disabled) {
            return;
          }
          onClick();
        }}
        style={{
          // borderRadius: active ? "40%" : "5px",
          // border: active ? "1px solid rgb(254, 254, 254)" : "1px solid #000000",
          color: active ? "rgb(255, 255, 255)" : "#000000",
          opacity: disabled ? 0.3 : active ? 1 : 0.8,
        }}
        disabled={disabled}
        active={active}
      >
        {label_letter}
      </MapMenuButtonStyle>
    </Tooltip>
  );
}

export default function MapMenuBar({
  setKeepCentered,
  keepCentered,
  setSelectingLaunchPosition,
  selectingLaunchPosition,
  setShowDistanceFromLaunch,
  showDistanceFromLaunch,
  setShowFlightPath,
  showFlightPath,
  isRunning,
}) {
  return (
    <MapMenuBarStyle>
      <MapMenuButton
        label_letter="C"
        onClick={() => {
          setKeepCentered(!keepCentered);
        }}
        tooltip_text={"Keep Centered on Flight"}
        active={keepCentered}
        disabled={false}
      />
      <MapMenuButton
        label_letter="D"
        onClick={() => {
          setShowDistanceFromLaunch(!showDistanceFromLaunch);
        }}
        tooltip_text={"Show Distance from Launch Position"}
        active={showDistanceFromLaunch}
        disabled={isRunning}
      />
      <MapMenuButton
        label_letter="F"
        onClick={() => {
          setShowFlightPath(!showFlightPath);
        }}
        tooltip_text={"Show Flight Path"}
        active={showFlightPath}
        disabled={isRunning}
      />
      <MapMenuButton
        label_letter="L"
        onClick={() => {
          setSelectingLaunchPosition(!selectingLaunchPosition);
        }}
        tooltip_text={"Set Launch Position. The mission clock must be stopped."}
        active={selectingLaunchPosition}
        disabled={isRunning}
      />
    </MapMenuBarStyle>
  );
}
