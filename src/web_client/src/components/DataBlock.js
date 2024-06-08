import React from "react";
import styled from "styled-components";

import Tooltip from "./Tooltip";

import {} from "giraffe-protocol";

import { useApiGetData } from "../api_interface/ggs_api";

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
  display: block;
  flex-direction: row;
  justify-content: space-between;
  width: 100%;
  align-items: center;
  text-align: center;
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
      Last updated: {time}s ago
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

  if (item_data.round !== undefined && typeof item_data.value === "number") {
    item_data.value = item_data.value.toFixed(item_data.round);
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

  // console.log(item_data);

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
  const { data, isDataLoading, error } = useApiGetData(
    resource,
    category,
    "all",
    update_interval
  );

  const [items, setItems] = React.useState({});
  const [secondsSinceLastUpdate, setSecondsSinceLastUpdate] = React.useState(0);

  // items structure:
  // {
  //  "data_item": {
  //    "name": "Nice Looking Name",
  //    "value": 123,
  //    "description": "A description of the data item"
  //    }
  // }

  React.useEffect(() => {
    if (isDataLoading || data === null || data === null) {
      return;
    }
    let new_items = {};
    if (category === "environmental") {
      console.log("env", data);
    }

    // iterate over the metadata and add the values
    for (const [data_item, meta] of Object.entries(data.metadata)) {
      // update the timestamp
      if (data_item === "MS_SINCE_LAST_UPDATE") {
        let sec = 0;
        if (typeof meta === "number") {
          sec = meta / 1000;
          sec = sec.toFixed(1);
        }
        setSecondsSinceLastUpdate(sec);
        continue;
      }

      new_items[data_item] = meta;

      // add the value if it exists
      if (data.values[data_item] !== undefined) {
        new_items[data_item]["value"] = data.values[data_item];
      } else {
        new_items[data_item]["value"] = "no-data";
      }

      // if the name is empty, use the data_item key
      if (new_items[data_item].name === "") {
        new_items[data_item].name = data_item;
      }
    }

    // iterate over the values and add any that don't have metadata
    for (const [data_item, value] of Object.entries(data.values)) {
      if (new_items[data_item] === undefined) {
        new_items[data_item] = {
          name: data_item,
          value: value,
          description: "",
        };
      }
    }

    // set the new items
    setItems(new_items);
  }, [data, isDataLoading, error, category]);

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
            time={secondsSinceLastUpdate}
            update_interval={update_interval}
          />
        </>
      )}
    </>
  );
}
