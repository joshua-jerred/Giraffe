// import styled from "styled-components";
import { useState, useEffect, useContext } from "react";
import { GGS_API } from "../api_interface/ggs_api";
import { GwsGlobal } from "../GlobalContext";

import { MapContainer, TileLayer, useMap, Circle } from "react-leaflet";

// const MapStyle = styled.div`
//   width: 100%;
//   height: 200px;
//   background-color: ${(props) => props.theme.colors.background};
// `;
export function Map() {
  const { ggsAddress } = useContext(GwsGlobal);
  const { ggsConnectionStatus } = useContext(GGS_API);

  const [position, setPosition] = useState({ lat: 0, lng: 0 }); // set default position

  const Recenter = ({ lat, lng }) => {
    const map = useMap();
    useEffect(() => {
      map.setView([lat, lng]);
    });
    return null;
  };

  useEffect(() => {
    const interval = setInterval(() => {
      fetch(`http://${ggsAddress}/api/flight_data/data?category=location`)
        .then((response) => response.json())
        .then((data) => {
          if (data.values.latitude === undefined) {
            return;
          }
          setPosition({
            lat: data.values.latitude,
            lng: data.values.longitude,
          });
        })
        .catch((error) => {
          console.log(error);
        });
    }, 1000);
    return () => clearInterval(interval);
  }, [ggsAddress, ggsConnectionStatus]);

  return (
    <div style={{ display: "block" }}>
      <MapContainer
        style={{
          height: "400px",
          width: "100%",
        }}
        center={position}
        zoom={8}
      >
        <Recenter lat={position.lat} lng={position.lng} />
        {position.lat !== 0 && position.lng !== 0 ? (
          <Circle center={position} radius={1000} />
        ) : null}
        {/* <Circle center={position} radius={2500} /> */}
        <TileLayer
          attribution="Google Maps"
          url="https://www.google.cn/maps/vt?lyrs=m@189&gl=cn&x={x}&y={y}&z={z}"
        />
      </MapContainer>
    </div>
  );
}
