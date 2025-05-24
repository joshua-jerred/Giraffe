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
  Polyline,
  Marker,
  Popup,
  useMapEvent,
  Tooltip,
} from "react-leaflet";

import { Icon } from "leaflet";

import {
  InputWithLabel,
  StySaveButton,
  StyNeutralButton,
} from "../../../components/styled/StyledComponents.js";

import { useApiGetData } from "../../../api_interface/ggs_api.js";

import { ApiRequestButton } from "../../../components/api_request_button.js";
import { use } from "react";

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
  latitude: 40.0,
  longitude: -100.0,
  altitude: 0.0,
};

const Recenter = ({ latitude, longitude, keepCentered, setCachedZoom }) => {
  const map = useMap();
  useEffect(() => {
    if (!keepCentered) {
      return;
    }
    map.setView([latitude, longitude]);
    setCachedZoom(map.getZoom());
  });
  return null;
};

function SelectPosition({
  selectingLaunchPosition,
  setlaunchPosition,
  launchPosition,
}) {
  const map = useMapEvent("click", (e) => {
    const { lat, lng } = e.latlng;

    console.log("zoom level:", map.getZoom());

    if (selectingLaunchPosition) {
      // console.log("Setting launch position to:", lat, lng);
      setlaunchPosition({
        valid: true,
        latitude: lat,
        longitude: lng,
        altitude:
          launchPosition.hasOwnProperty("altitude") &&
          !isNaN(launchPosition.altitude)
            ? launchPosition.altitude
            : 0,
      });
    }
  });
  return null;
}

function SelectLaunchPositionForm({
  launchPosition,
  setlaunchPosition,
  setSelectingLaunchPosition,
}) {
  return (
    <div
      style={{
        display: "flex",
        width: "100%",
        justifyContent: "space-around",
        alignItems: "center",
        gap: "10px",
        marginTop: "10px",
        flexWrap: "wrap",
        flexDirection: "row",
      }}
    >
      <InputWithLabel
        label="latitude dd.dddd"
        type="number"
        value={launchPosition.latitude}
        onChange={(e) => {
          let value = parseFloat(e.target.value);
          if (isNaN(value)) {
            value = 0.0;
          }
          setlaunchPosition({
            ...launchPosition,
            latitude: value,
          });
        }}
        placeholder="Latitude"
        // style={{ width: "120px" }}
        inputStyle={{ width: "120px" }}
      />
      <InputWithLabel
        label="longitude ddd.dddd"
        type="number"
        value={launchPosition.longitude}
        onChange={(e) => {
          let value = parseFloat(e.target.value);
          if (isNaN(value)) {
            value = 0.0;
          }
          setlaunchPosition({
            ...launchPosition,
            longitude: value,
          });
        }}
        placeholder="Longitude"
        inputStyle={{ width: "120px" }}
      />
      <InputWithLabel
        label="altitude m"
        type="number"
        value={launchPosition.altitude}
        onChange={(e) => {
          let value = parseFloat(e.target.value);
          if (isNaN(value)) {
            value = 0.0;
          }
          setlaunchPosition({
            ...launchPosition,
            altitude: value,
          });
        }}
        placeholder="meters"
        inputStyle={{ width: "120px" }}
      />
      <ApiRequestButton
        api_endpoint="/api/flight_data/data?category=set_launch_position"
        title="save"
        request_data_callback={() => {
          return {
            latitude: launchPosition.latitude,
            longitude: launchPosition.longitude,
            altitude: launchPosition.altitude,
          };
        }}
        success_callback={() => {
          console.log("Saved launch position");
          setSelectingLaunchPosition(false);
        }}
        // api_method="POST"
        api_method="PUT"
        style={{
          padding: "0em 0.7em",
          flexGrow: 1,
        }}
      />
      <StyNeutralButton
        onClick={() => {
          setSelectingLaunchPosition(false);
        }}
        style={{
          padding: "0em 0.7em",
          flexGrow: 1,
        }}
      >
        cancel
      </StyNeutralButton>
    </div>
  );
}

function PositionDataPopup({ title, positionData }) {
  return (
    <div>
      <h4>{title}</h4>
      {/* <br /> */}
      Valid: {JSON.stringify(positionData.valid)}
      <br />
      Lat:{" "}
      {typeof positionData.latitude === "number"
        ? positionData.latitude.toFixed(7)
        : positionData.latitude}
      <br />
      Lng:
      {typeof positionData.longitude === "number"
        ? positionData.longitude.toFixed(7)
        : positionData.longitude}
      <br />
      Alt: {positionData.altitude} m
      <br />
      {positionData.hasOwnProperty("heading") && "Hdg: " + positionData.heading}
      <br />
      {positionData.hasOwnProperty("horizontal_speed") &&
        positionData.horizontal_speed !== "n/d" &&
        "h/s: " + positionData.horizontal_speed + " m/s"}
      <br />
      {positionData.hasOwnProperty("vertical_speed") &&
        positionData.vertical_speed !== "n/d" &&
        "v/s: " + positionData.vertical_speed + " m/s"}
      <br />
      {positionData.timestamp}
      <br />
      {positionData.hasOwnProperty("additional_data") &&
        positionData.additional_data}
      <br />
      {positionData.hasOwnProperty("source") &&
        "Source: " + positionData.source}
    </div>
  );
}

function FlightPath() {
  const { data, isLoading, error, setNeedUpdate } = useApiGetData(
    "flight_data",
    "flight_path",
    "all",
    1000
  );

  const [flightPath, setFlightPath] = useState([[0, 0]]);

  // Load the location data
  useEffect(() => {
    if (isLoading) {
      return;
    } else if (error) {
      console.error("Error loading flight path data:", error);
    } else if (data) {
      // console.log("Flight path data loaded:", data);
      let flight_path = [].concat(data).map((data) => {
        return [data.latitude, data.longitude];
      });
      setFlightPath(flight_path);
    }
  }, [data, isLoading, error]);

  return (
    <Polyline
      positions={flightPath}
      color="black"
      weight={3}
      opacity={0.8}
      // smoothFactor={1}
    ></Polyline>
  );
}

export default function MissionMap() {
  // const { ggsAddress } = useContext(GwsGlobal);
  const { isRunning } = useMissionClockData();
  const [statusText, setStatusText] = useState("Loading...");

  const [savedZoom, setSavedZoom] = useStorageState(
    "command-center-map-zoom-level",
    8
  );

  const [keepCentered, setKeepCentered] = useStorageState(
    "command-center-map-keep-centered",
    true
  );
  const [showDistanceFromLaunch, setShowDistanceFromLaunch] = useStorageState(
    "command-center-map-show-distance-from-launch",
    true
  );
  const [showFlightPath, setShowFlightPath] = useStorageState(
    "command-center-map-show-flight-path",
    true
  );

  const [selectingLaunchPosition, setSelectingLaunchPosition] = useState(false);
  const [launchPosition, setlaunchPosition] = useState(NULL_POS);
  // const [groundStationPosition, setGroundStationPosition] = useState(NULL_POS);
  const [flightPosition, setFlightPosition] = useState(NULL_POS);
  const [distanceFromLaunch, setDistanceFromLaunch] = useState("n/d");

  const { data, isLoading, error, setNeedUpdate } = useApiGetData(
    "flight_data",
    "location",
    "all",
    1000
  );

  // Effect for selecting launch position
  useEffect(() => {
    if (selectingLaunchPosition) {
      setStatusText("Select the Launch Position");
    } else {
      setStatusText();
    }
  }, [selectingLaunchPosition]);

  // Load the location data
  useEffect(() => {
    if (isLoading) {
      // setStatusText("Loading...");
      // don't set status text, we're on an interval
    } else if (error) {
      setStatusText("Error: " + error);
    } else if (data) {
      setStatusText("");

      try {
        // let metadata = data.metadata;
        let launch_position = data.values.launch_position;
        if (!selectingLaunchPosition) {
          setlaunchPosition(launch_position);
        }
        let flight_position = data.values.flight_position;
        if (flight_position) {
          setFlightPosition(flight_position);
        }
        if (data.values.hasOwnProperty("distance_from_launch_position_km")) {
          setDistanceFromLaunch(data.values.distance_from_launch_position_km);
        }
        // let ground_station_position = data.values.ground_station_position;
      } catch (e) {
        console.error("Error parsing flight data:", e);
        setStatusText("Error parsing flight data");
      }
    }
  }, [data, isLoading, error, selectingLaunchPosition]);

  return (
    <div style={{ display: "block" }}>
      <MapMenuBar
        setKeepCentered={setKeepCentered}
        keepCentered={keepCentered}
        setSelectingLaunchPosition={setSelectingLaunchPosition}
        selectingLaunchPosition={selectingLaunchPosition}
        setShowDistanceFromLaunch={setShowDistanceFromLaunch}
        showDistanceFromLaunch={showDistanceFromLaunch}
        setShowFlightPath={setShowFlightPath}
        showFlightPath={showFlightPath}
        isRunning={isRunning}
      />
      <MapContainer
        style={{
          height: "400px",
          width: "100%",
        }}
        center={{ lat: flightPosition.latitude, lng: flightPosition.longitude }}
        zoom={keepCentered ? savedZoom : 3}
      >
        <Recenter
          latitude={flightPosition.latitude}
          longitude={flightPosition.longitude}
          keepCentered={keepCentered}
          setCachedZoom={setSavedZoom}
        />
        <SelectPosition
          selectingLaunchPosition={selectingLaunchPosition}
          setlaunchPosition={setlaunchPosition}
          launchPosition={launchPosition}
        />

        {/* ---------------
            Launch Position
            --------------- */}
        {selectingLaunchPosition ? (
          <DraggableMarker
            position={launchPosition}
            setPosition={setlaunchPosition}
            draggable={selectingLaunchPosition}
            popupText={
              <PositionDataPopup
                title="Launch Position"
                positionData={launchPosition}
              />
            }
            textWhenDraggable={"Click or drag to set launch position"}
            extraTooltip={
              !launchPosition.valid && !selectingLaunchPosition
                ? "Invalid/Unset Launch Position"
                : null
            }
            icon={
              new Icon({
                iconUrl: "/leaflet/launch_position.svg",
                iconSize: [50, 50],
                iconAnchor: [12.5, 12.5],
              })
            }
          />
        ) : (
          <Marker
            position={{
              lat: launchPosition.latitude,
              lng: launchPosition.longitude,
            }}
            icon={
              new Icon({
                iconUrl: "/leaflet/launch_position.svg",
                iconSize: [40, 40],
              })
            }
          >
            <Popup>
              <PositionDataPopup
                title="Launch Position"
                positionData={launchPosition}
              />
            </Popup>
          </Marker>
        )}

        {/* ---------------
            Flight Position
            --------------- */}
        <Marker
          position={{
            lat: flightPosition.latitude,
            lng: flightPosition.longitude,
          }}
          icon={
            new Icon({
              iconUrl: "/leaflet/flight_position.svg",
              iconSize: [30, 30],
            })
          }
        >
          <Popup>
            <PositionDataPopup
              title="Flight Position"
              positionData={flightPosition}
            />
          </Popup>
        </Marker>
        {/* <Circle center={position} radius={2500} /> */}

        {showDistanceFromLaunch && (
          <Polyline
            positions={[
              [launchPosition.latitude, launchPosition.longitude],
              [flightPosition.latitude, flightPosition.longitude],
            ]}
            color="blue"
            weight={5}
            opacity={0.5}
            smoothFactor={1}
          >
            <Tooltip>
              <p>Distance From Launch {distanceFromLaunch} km</p>
            </Tooltip>
          </Polyline>
        )}

        {showFlightPath && <FlightPath showFlightPath={showFlightPath} />}

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
      {selectingLaunchPosition ? (
        <SelectLaunchPositionForm
          launchPosition={launchPosition}
          setlaunchPosition={setlaunchPosition}
          setSelectingLaunchPosition={setSelectingLaunchPosition}
        />
      ) : null}
    </div>
  );
}
