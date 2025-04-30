import { useState, useRef, useMemo, useCallback } from "react";

import { Circle, Marker, Popup, Tooltip } from "react-leaflet";

export default function DraggableMarker({
  position,
  setPosition,
  draggable,
  popupText,
  textWhenDraggable,
}) {
  const markerRef = useRef(null);
  const eventHandlers = useMemo(
    () => ({
      dragend() {
        const marker = markerRef.current;
        if (marker != null) {
          setPosition({
            ...position,
            lat: marker.getLatLng().lat,
            lng: marker.getLatLng().lng,
          });
        }
      },
    }),
    []
  );

  return (
    <Marker
      draggable={draggable}
      eventHandlers={eventHandlers}
      position={position}
      ref={markerRef}
    >
      {draggable ? (
        <>
          <Tooltip permanent>{textWhenDraggable}</Tooltip>
          <Circle
            center={position}
            radius={100}
            pathOptions={{
              color: "black",
              stroke: true,
              // fillColor: "blue",
              // fillOpacity: 0.2,
              fill: false,
            }}
          />
        </>
      ) : null}
      <Circle
        center={position}
        radius={10}
        pathOptions={{
          color: "black",
          fillColor: "red",
          fillOpacity: 0.5,
          stroke: true,
          fill: draggable,
          strokeWidth: 2,
        }}
      />
      <Popup minWidth={90}>
        <span>{popupText}</span>
      </Popup>
    </Marker>
  );
}
