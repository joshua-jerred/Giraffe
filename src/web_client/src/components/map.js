import styled from "styled-components";

import { MapContainer, TileLayer } from "react-leaflet";

const MapStyle = styled.div`
  width: 100%;
  height: 200px;
  background-color: ${(props) => props.theme.colors.background};
`;
export function Map() {
  return (
    <div style={{ display: "block" }}>
      <MapContainer
        style={{
          height: "500px",
          width: "100%",
        }}
        center={[39.505972, -101.977203]}
        zoom={6}
      >
        <TileLayer
          attribution="Google Maps"
          url="https://www.google.cn/maps/vt?lyrs=m@189&gl=cn&x={x}&y={y}&z={z}"
        />
      </MapContainer>
    </div>
  );
}
