import React from "react";
import styled from "styled-components";

import Tooltip from "./Tooltip";

import {} from "giraffe-protocol";

import { GGS_API } from "../api_interface/ggs_api";
import { GwsGlobal } from "../GlobalContext";

const DataBoxContainer = styled.div`
  // list-style: none;
  // padding: 3px 3px;
  // margin: 0;
`;

const ItemStyle = styled.div`
  display: flex;
  flex-direction: row;
  justify-content: space-between;
  margin-bottom: 0px;
  margin-top: 0px;
  height: calc(${(props) => props.theme.components.input.height} * 0.85);
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
  text-overflow: ellipsis;
  overflow: hidden;
  white-space: nowrap;
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

const TimestampStyle = styled.div`
  font-size: 0.8em;
  color: ${(props) => {
    if (props.time <= props.update_interval) {
      return props.theme.on_surface;
    } else if (props.time > props.update_interval * 4) {
      return props.theme.error;
    } else {
      return props.theme.warning;
    }
  }};
`;
function Timestamp({ time, update_interval }) {
  return (
    <TimestampStyle time={time} update_interval={update_interval}>
      Last updated: {time}ms ago
    </TimestampStyle>
  );
}

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

export function DataBlock({ resource, category, update_interval = 3000 }) {
  const { ggsAddress } = React.useContext(GwsGlobal);
  const { ggsConnectionStatus } = React.useContext(GGS_API);

  const [items, setItems] = React.useState({});
  const [error, setError] = React.useState(null);

  const [msSinceLastUpdate, setMsSinceLastUpdate] = React.useState(0);

  const encoded_category = encodeURIComponent(category);
  const path = `http://${ggsAddress}/api/${resource}/data?category=${encoded_category}`;

  // First load the metadata
  React.useEffect(() => {
    // console.log("Loading metadata from: " + path);
    fetch(path + "&include=metadata")
      .then((response) => {
        if (!response.ok) {
          throw new Error("Failed to load metadata.");
        } else {
          return response.json();
        }
      })
      .then((data) => {
        delete data.metadata.MS_SINCE_LAST_UPDATE;

        let new_items = {};
        for (const [key, meta] of Object.entries(data.metadata)) {
          new_items[key] = meta;
        }
        for (const [key, value] of Object.entries(data.metadata)) {
          if (new_items[key] === undefined) {
            new_items[key] = { value: value.default };
          }
          new_items[key].meta = value;
        }
        setItems(new_items);
        // sendStreamRequest(category);
      })
      .catch((error) => {
        console.error(error);
        setError("Failed to load metadata. (Check console for details.)");
      });
  }, [path, ggsConnectionStatus]);

  // Then load the actual data
  React.useEffect(() => {
    const loadData = async () => {
      fetch(path)
        .then((response) => {
          if (!response.ok) {
            throw new Error("Failed to load data.");
          } else {
            return response.json();
          }
        })
        .then((data) => {
          setMsSinceLastUpdate(data.metadata.MS_SINCE_LAST_UPDATE);
          // console.log("ms: ", msSinceLastUpdate);
          let new_items = data.values;
          let old_items = items;
          for (const [key, value] of Object.entries(new_items)) {
            if (old_items[key] === undefined) {
              old_items[key] = { value: value };
            } else {
              old_items[key].value = value;
            }
          }
          setItems(old_items);
          if (error !== null) {
            setError(null);
          }
        })
        .catch((error) => {
          console.error(error);
          setError("Failed to load actual data. (Check console for details.)");
        });
    };

    loadData(); // run once immediately
    const interval = setInterval(() => {
      loadData();
    }, update_interval);
    return () => clearInterval(interval);
  }, [path, ggsConnectionStatus]);

  // if (ggsConnectionStatus !== true) {
  // return <div>Not connected to GGS.</div>;
  // }

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
          <Timestamp
            time={msSinceLastUpdate}
            update_interval={update_interval}
          />
        </>
      )}
    </>
  );
}
