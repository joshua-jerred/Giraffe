// import {
//   Cartesian3,
//   createOsmBuildingsAsync,
//   Ion,
//   Math as CesiumMath,
//   Terrain,
//   Viewer,
// } from "cesium";
import "cesium/Build/Cesium/Widgets/widgets.css";
import { Cartesian3 } from "cesium";
import { Viewer, Entity } from "cesium-react";

// The URL on your server where CesiumJS's static files are hosted.
// window.CESIUM_BASE_URL = "/";

// Your access token can be found at: https://ion.cesium.com/tokens.
// Replace `your_access_token` with your Cesium ion access token.

// Ion.defaultAccessToken =
// "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiIyNGMyMzgyMS04YTRjLTRjN2MtYWY0MS0zNDY5MzUyMDgzY2UiLCJpZCI6Mjk4NjU2LCJpYXQiOjE3NDYwMzMxMzh9.8lQ0L6Yh35jHIwbQYZKS1xN0-syV1TmKZTddz4_WDJw";

// Initialize the Cesium Viewer in the HTML element with the `cesiumContainer` ID.
// const viewer = new Viewer("cesiumContainer", {
// terrain: Terrain.fromWorldTerrain(),
// });

// Fly the camera to San Francisco at the given longitude, latitude, and height.
// viewer.camera.flyTo({
// destination: Cartesian3.fromDegrees(-122.4175, 37.655, 400),
// orientation: {
// heading: CesiumMath.toRadians(0.0),
// pitch: CesiumMath.toRadians(-15.0),
// },
// });

// Add Cesium OSM Buildings, a global 3D buildings layer.
// const buildingTileset = await createOsmBuildingsAsync();
// viewer.scene.primitives.add(buildingTileset);

//////

// // import styled from "styled-components";
// import { useState, useEffect, useContext } from "react";
// // import { GGS_API } from "../api_interface/ggs_api";
// import { GwsGlobal } from "../GlobalContext";

// import { MapContainer, TileLayer, useMap, Circle } from "react-leaflet";

// // const MapStyle = styled.div`
// //   width: 100%;
// //   height: 200px;
// //   background-color: ${(props) => props.theme.colors.background};
// // `;
export function Map() {
  //   const { ggsAddress } = useContext(GwsGlobal);
  //   // const { ggsConnectionStatus } = useContext(GGS_API);

  //   const [launchPosition, setLaunchPosition] = useState(null);
  //   const [position, setPosition] = useState({ lat: 0, lng: 0 }); // set default position

  //   const Recenter = ({ lat, lng }) => {
  //     const map = useMap();
  //     useEffect(() => {
  //       map.setView([lat, lng]);
  //     });
  //     return null;
  //   };

  //   useEffect(() => {
  //     const interval = setInterval(() => {
  //       fetch(`${ggsAddress}/api/flight_data/data?category=location`)
  //         .then((response) => response.json())
  //         .then((data) => {
  //           if (data.values.latitude === undefined) {
  //             return;
  //           }
  //           setPosition({
  //             lat: data.values.latitude,
  //             lng: data.values.longitude,
  //           });
  //           if (data.values.launch_position !== undefined) {
  //             setLaunchPosition({
  //               lat: data.values.launch_position.latitude,
  //               lng: data.values.launch_position.longitude,
  //             });
  //           }
  //         })
  //         .catch((error) => {
  //           console.log(error);
  //         });
  //     }, 1000);
  //     return () => clearInterval(interval);
  //   }, [ggsAddress]);

  return (
    <div style={{ display: "block" }}>
      <Viewer full>
        <Entity
          name="tokyo"
          position={Cartesian3.fromDegrees(139.767052, 35.681167, 100)}
          point={{ pixelSize: 10 }}
        >
          test
        </Entity>
      </Viewer>
    </div>
  );
}

//       <MapContainer
//         style={{
//           height: "400px",
//           width: "100%",
//         }}
//         center={position}
//         zoom={8}
//       >
//         <Recenter lat={position.lat} lng={position.lng} />
//         {position.lat !== 0 && position.lng !== 0 ? (
//           <Circle center={position} radius={1000} />
//         ) : null}
//         {launchPosition !== null ? (
//           <Circle
//             center={launchPosition}
//             radius={10}
//             pathOptions={{ color: "red" }}
//           />
//         ) : null}
//         {/* <Circle center={position} radius={2500} /> */}
//         <TileLayer
//           attribution="Google Maps"
//           url="https://www.google.cn/maps/vt?lyrs=m@189&gl=cn&x={x}&y={y}&z={z}"
//         />
//       </MapContainer>
