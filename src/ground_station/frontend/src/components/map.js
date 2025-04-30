import styled from "styled-components";
import { useState, useEffect, useContext } from "react";
// import { GGS_API } from "../api_interface/ggs_api";
import { GwsGlobal } from "../GlobalContext";

import { MapContainer, TileLayer, useMap, Circle, Marker } from "react-leaflet";

// const MapConfig = {
//   center: [40, -100],
//   zoom: 8,
// };

// const MapStyle = styled.div`
//   width: 100%;
//   height: 200px;
//   background-color: ${(props) => props.theme.colors.background};
// `;

const MapMenuBar = styled.div`
  display: flex;
  justify-content: space-between;
  align-items: center;
  background-color: ${(props) => props.theme.surface};
  padding: 10px;
`;

export function Map() {
  const { ggsAddress } = useContext(GwsGlobal);
  // const { ggsConnectionStatus } = useContext(GGS_API);

  const [groundStationPosition, setGroundStationPosition] = useState({
    lat: 40,
    lng: -100,
  });
  const [launchPosition, setLaunchPosition] = useState(null);
  const [position, setPosition] = useState({ lat: 40, lng: -100 }); // set default position

  const Recenter = ({ lat, lng }) => {
    const map = useMap();
    useEffect(() => {
      map.setView([lat, lng]);
    });
    return null;
  };

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

  return (
    <div style={{ display: "block" }}>
      <MapMenuBar />
      <MapContainer
        style={{
          height: "400px",
          width: "100%",
        }}
        center={position}
        zoom={2}
      >
        <Recenter lat={position.lat} lng={position.lng} />
        {position.lat !== 0 && position.lng !== 0 ? (
          <Circle center={position} radius={1000} />
        ) : null}
        {launchPosition !== null ? (
          <Circle
            center={launchPosition}
            radius={10}
            pathOptions={{ color: "red" }}
          />
        ) : null}
        <Marker position={groundStationPosition}>
          <p>Ground Station Location</p>
        </Marker>
        {/* <Circle center={position} radius={2500} /> */}
        <TileLayer
          attribution="Google Maps"
          url="https://www.google.cn/maps/vt?lyrs=m@189&gl=cn&x={x}&y={y}&z={z}"
        />
      </MapContainer>
    </div>
  );
}
