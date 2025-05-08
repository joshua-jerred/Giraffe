import { useState, useRef, useMemo, useCallback } from "react";

import { Circle, Marker, Popup, Tooltip } from "react-leaflet";

export default function DraggableMarker({
  position,
  setPosition,
  draggable,
  popupText,
  textWhenDraggable,
  extraTooltip = null,
}) {
  const markerRef = useRef(null);
  const eventHandlers = useMemo(
    () => ({
      dragend() {
        const marker = markerRef.current;
        if (marker != null) {
          setPosition({
            ...position,
            latitude: marker.getLatLng().lat,
            longitude: marker.getLatLng().lng,
          });
        }
      },
    }),
    []
  );

  return (
    <Marker
      // icon={null}
      draggable={draggable}
      eventHandlers={eventHandlers}
      position={{
        lat: position.latitude,
        lng: position.longitude,
      }}
      size={[50, 200]}
      ref={markerRef}
      interactive
      riseOnHover
    >
      <div
        style={{
          background: "red",
          textAlign: "center",
        }}
      ></div>
      {draggable ? (
        <>
          <Tooltip permanent>{textWhenDraggable}</Tooltip>
          <Circle
            center={{
              lat: position.latitude,
              lng: position.longitude,
            }}
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
      ) : (
        extraTooltip && <Tooltip permanent>{extraTooltip}</Tooltip>
      )}
      <Circle
        center={{
          lat: position.latitude,
          lng: position.longitude,
        }}
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
      <Popup minWidth={90}>{popupText}</Popup>
    </Marker>
  );
}
