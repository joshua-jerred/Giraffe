import styled from "styled-components";
import { useState, useEffect, useContext } from "react";
// import { GGS_API } from "../api_interface/ggs_api";
import { GwsGlobal } from "../../../GlobalContext.js";
// import Tooltip from "../../../components/Tooltip.js";
import { useStorageState } from "../../../core/LocalStorageState.js";
import { useMissionClockData } from "../../../api_interface/mission_clock_api.js";
import MapMenuBar from "./map_menu_bar.js";
import DraggableMarker from "./draggable_marker.js";

import {
  MapContainer,
  TileLayer,
  useMap,
  Circle,
  Marker,
  Popup,
  useMapEvent,
} from "react-leaflet";

import { InputWithLabel } from "../../../components/styled/StyledComponents.js";

// const MapConfig = {
//   center: [40, -100],
//   zoom: 8,
// };

// const MapStyle = styled.div`
//   width: 100%;
//   height: 200px;
//   background-color: ${(props) => props.theme.colors.background};
// `;

const NULL_POS = {
  valid: false,
  lat: 40,
  lng: -100,
  alt: 0,
};

const Recenter = ({ lat, lng, keepCentered }) => {
  const map = useMap();
  useEffect(() => {
    if (!keepCentered) {
      return;
    }
    map.setView([lat, lng]);
  });
  return null;
};

function SelectPosition({ selectingLaunchPosition, setLaunchPositionBuffer }) {
  const map = useMapEvent("click", (e) => {
    const { lat, lng } = e.latlng;

    if (selectingLaunchPosition) {
      console.log("Setting launch position to:", lat, lng);
      setLaunchPositionBuffer({
        valid: true,
        lat: lat,
        lng: lng,
        alt: 0,
      });
    }
  });
  return null;
}

export default function MissionMap() {
  const { ggsAddress } = useContext(GwsGlobal);
  const { isRunning } = useMissionClockData();
  const [statusText, setStatusText] = useState("");

  const [keepCentered, setKeepCentered] = useStorageState(
    "command-center-map-keep-centered",
    true
  );
  const [selectingLaunchPosition, setSelectingLaunchPosition] = useState(false);

  const [launchPositionBuffer, setLaunchPositionBuffer] = useState(NULL_POS);
  const [launchPosition, setLaunchPosition] = useState(NULL_POS);
  const [groundStationPosition, setGroundStationPosition] = useState(NULL_POS);
  const [flightPosition, setFlightPosition] = useState(NULL_POS);

  // useEffect(() => {
  //   const interval = setInterval(() => {
  //     fetch(`${ggsAddress}/api/flight_data/data?category=location`)
  //       .then((response) => response.json())
  //       .then((data) => {
  //         console.log("Location data:", data);
  //         if (data.values.latitude === undefined) {
  //           return;
  //         }
  //         setPosition({
  //           lat: data.values.latitude,
  //           lng: data.values.longitude,
  //         });
  //         if (data.values.launch_position !== undefined) {
  //           setLaunchPosition({
  //             lat: data.values.launch_position.latitude,
  //             lng: data.values.launch_position.longitude,
  //           });
  //         }
  //       })
  //       .catch((error) => {
  //         console.log(error);
  //       });
  //   }, 1000);
  //   return () => clearInterval(interval);
  // }, [ggsAddress]);

  useEffect(() => {
    if (selectingLaunchPosition) {
      setStatusText("Select the Launch Position");
    } else {
      setStatusText();
    }
  }, [selectingLaunchPosition]);

  return (
    <div style={{ display: "block" }}>
      <MapMenuBar
        setKeepCentered={setKeepCentered}
        keepCentered={keepCentered}
        setSelectingLaunchPosition={setSelectingLaunchPosition}
        selectingLaunchPosition={selectingLaunchPosition}
        isRunning={isRunning}
      />
      <MapContainer
        style={{
          height: "400px",
          width: "100%",
        }}
        center={flightPosition}
        zoom={4}
      >
        <Recenter
          lat={flightPosition.lat}
          lng={flightPosition.lng}
          keepCentered={keepCentered}
        />
        <SelectPosition
          selectingLaunchPosition={selectingLaunchPosition}
          setLaunchPositionBuffer={setLaunchPositionBuffer}
        />

        {/* {flightPosition.lat !== 0 && flightPosition.lng !== 0 ? (
          <Circle center={flightPosition} radius={1000} />
        ) : null} */}
        {/* {launchPosition !== null ? (
          <Circle
            center={launchPosition}
            radius={10}
            pathOptions={{ color: "red" }}
          />
        ) : null} */}
        <DraggableMarker
          position={launchPositionBuffer}
          setPosition={setLaunchPositionBuffer}
          draggable={selectingLaunchPosition}
          popupText={"Launch Position"}
          textWhenDraggable={"Click or drag to set launch position"}
        />
        {/* <Marker position={groundStationPosition}> */}
        {/* <Popup> */}
        {/* <p>Ground Station Location</p> */}
        {/* </Popup> */}
        {/* </Marker> */}
        {/* <Circle center={position} radius={2500} /> */}
        <TileLayer
          // attribution="Google Maps"
          // url="https://www.google.cn/maps/vt?lyrs=m@189&gl=cn&x={x}&y={y}&z={z}"
          attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a>'
          url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        />
      </MapContainer>
      <div
        style={{
          display: "flex",
          width: "100%",
          justifyContent: "center",
          alignItems: "center",
        }}
      >
        {statusText}
      </div>

      <div
        style={{
          display: "flex",
          width: "100%",
          justifyContent: "center",
          alignItems: "center",
          gap: "10px",
          marginTop: "10px",
        }}
      >
        <InputWithLabel
          label="latitude dd.dddd"
          type="number"
          value={launchPositionBuffer.lat}
          onChange={(e) => {
            setLaunchPositionBuffer({
              ...launchPositionBuffer,
              lat: parseFloat(e.target.value),
            });
          }}
          placeholder="Latitude"
        />
        <InputWithLabel
          label="longitude ddd.dddd"
          type="number"
          value={launchPositionBuffer.lng}
          onChange={(e) => {
            setLaunchPositionBuffer({
              ...launchPositionBuffer,
              lng: parseFloat(e.target.value),
            });
          }}
          placeholder="Longitude"
        />
        <InputWithLabel
          label="altitude m"
          type="number"
          value={launchPositionBuffer.alt}
          onChange={(e) => {
            setLaunchPositionBuffer({
              ...launchPositionBuffer,
              alt: parseFloat(e.target.value),
            });
          }}
          placeholder="Altitude"
        />
      </div>
    </div>
  );
}
