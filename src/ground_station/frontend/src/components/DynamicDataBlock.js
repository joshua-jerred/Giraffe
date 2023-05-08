import React from "react";
import styled from "styled-components";

import Tooltip from "./Tooltip";

import {} from "giraffe-protocol";

import { GGS_WS } from "../api_interface/ws_api";
import { GwsGlobal } from "../GlobalContext";

const DataBoxContainer = styled.form`
  list-style: none;
  padding: 3px 3px;
  margin: 0;
`;

const ItemStyle = styled.li`
  display: flex;
  flex-direction: row;
  justify-content: space-between;
  margin-bottom: ${(props) => props.theme.components.edit_box.margin_botton};
  height: calc(${(props) => props.theme.components.input.height} * 1.5);
`;

const ItemName = styled.span`
  font-weight: bold;
  width: 50%;
  text-align: left;
  min-width: fit-content;
`;

const ItemValue = styled.span`
  width: 50%;
  text-align: right;
`;

const DataTooltip = styled(Tooltip)`
  width: 100%;
`;

const DataBoxStatus = styled.div`
  display: flex;
  flex-direction: row;
  justify-content: space-between;
  align-items: center;
`;

function Item({ id, item_data }) {
  if (id === undefined && item_data.name === undefined) {
    return (
      <ItemStyle>
        <span>Invalid item</span>
      </ItemStyle>
    );
  }

  if (item_data.name === undefined) {
    item_data.name = id;
  }

  let display_value = item_data.value;
  if (typeof item_data.value === "boolean") {
    if (item_data.true === undefined) {
      display_value = item_data.value ? "true" : "false";
    } else {
      display_value = item_data.value ? item_data.true : item_data.false;
    }
  } else if (typeof item_data.value === "object") {
    display_value = JSON.stringify(item_data.value);
  }

  return (
    <ItemStyle>
      <ItemName>
        <DataTooltip text={item_data.description}>{item_data.name}</DataTooltip>
      </ItemName>
      <ItemValue>{display_value}</ItemValue>
    </ItemStyle>
  );
}

export function DataStreamBlock({ resource, category, stream_name }) {
  const { ggsAddress } = React.useContext(GwsGlobal);
  const { ggsConnectionStatus, sendStreamRequest, lastJsonMessage } =
    React.useContext(GGS_WS);

  const [items, setItems] = React.useState({});
  const [error, setError] = React.useState(null);

  const encoded_category = encodeURIComponent(category);
  const path = `http://${ggsAddress}/api/${resource}/data?category=${encoded_category}`;
  React.useEffect(() => {
    console.log("Loading metadata from: " + path);
    fetch(path)
      .then((response) => {
        if (!response.ok) {
          throw new Error("Failed to load metadata.");
        } else {
          return response.json();
        }
      })
      .then((data) => {
        let new_items = {};
        for (const [key, value] of Object.entries(data.values)) {
          new_items[key] = { value: value };
        }
        for (const [key, value] of Object.entries(data.metadata)) {
          if (new_items[key] === undefined) {
            new_items[key] = { value: value.default };
          }
          new_items[key].meta = value;
        }
        setItems(new_items);
        sendStreamRequest(stream_name);
      })
      .catch((error) => {
        console.error(error);
        setError("Failed to load metadata. (Check console for details.)");
      });
  }, [path]);

  React.useEffect(() => {
    const stream = lastJsonMessage;
    if (stream === undefined || stream === null) {
      return;
    } else if (stream.cat === "stream" && stream.id === stream_name) {
      let new_items = stream.body;
      let old_items = items;
      for (const [key, value] of Object.entries(new_items)) {
        if (old_items[key] === undefined) {
          old_items[key] = { value: value };
        } else {
          old_items[key].value = value;
        }
      }
      setItems(old_items);
    }
  }, [lastJsonMessage]);

  if (ggsConnectionStatus !== "connected") {
    return <div>Not connected to GGS.</div>;
  }

  return (
    <>
      {error != null ? (
        <DataBoxStatus>{error}</DataBoxStatus>
      ) : items === null || items === undefined ? (
        <DataBoxStatus>Loading Data...</DataBoxStatus>
      ) : (
        <>
          <DataBoxContainer>
            {Object.entries(items).map(([key, value]) => (
              <Item id={key} key={key} item_data={value} />
            ))}
          </DataBoxContainer>
        </>
      )}
    </>
  );
}

export function NestedDataStreamBlock({
  resource,
  category,
  stream_name,
  nested,
}) {
  const { ggsAddress } = React.useContext(GwsGlobal);
  const { ggsConnectionStatus, sendStreamRequest, lastJsonMessage } =
    React.useContext(GGS_WS);

  const [items, setItems] = React.useState({});
  const [error, setError] = React.useState(null);

  const encoded_category = encodeURIComponent(category);
  const path = `http://${ggsAddress}/api/${resource}/data?category=${encoded_category}`;
  React.useEffect(() => {
    console.log("Loading metadata from: " + path);
    fetch(path)
      .then((response) => {
        if (!response.ok) {
          throw new Error("Failed to load metadata.");
        } else {
          return response.json();
        }
      })
      .then((data) => {
        let new_items = {};
        for (const [key, value] of Object.entries(data.values)) {
          for (const [key2, value2] of Object.entries(value)) {
          new_items[key2] = { value: value2 };
          }
        }
        for (const [key, value] of Object.entries(data.metadata)) {
          for (const [key2, value2] of Object.entries(value)) {
            if (new_items[key2] === undefined) {
              new_items[key2] = { value: value2.default };
            }
            new_items[key2].meta = value2;
          }
        }
        setItems(new_items);
        sendStreamRequest(stream_name);
      })
      .catch((error) => {
        console.error(error);
        setError("Failed to load metadata. (Check console for details.)");
      });
  }, [path]);

  React.useEffect(() => {
    const stream = lastJsonMessage;
    if (stream === undefined || stream === null) {
      return;
    } else if (stream.cat === "stream" && stream.id === stream_name) {
      let new_categories = stream.body;
      let old_items = items;
      for (const [key, value] of Object.entries(new_categories)) {
        for (const [key2, value2] of Object.entries(value)) {
          if (old_items[key2] === undefined) {
            old_items[key2] = { value: value2 };
          } else {
            old_items[key2].value = value2;
          }
        }
      }
      setItems(old_items);
    }
  }, [lastJsonMessage]);

  if (ggsConnectionStatus !== "connected") {
    return <div>Not connected to GGS.</div>;
  }

  return (
    <>
      {error != null ? (
        <DataBoxStatus>{error}</DataBoxStatus>
      ) : items === null || items === undefined ? (
        <DataBoxStatus>Loading Data...</DataBoxStatus>
      ) : (
        <>
          <DataBoxContainer>
            {Object.entries(items).map(([key, value]) => (
              <Item id={key} key={key} item_data={value} />
            ))}
          </DataBoxContainer>
        </>
      )}
    </>
  );
}
