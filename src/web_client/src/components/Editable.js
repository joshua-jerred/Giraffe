import React from "react";
import styled from "styled-components";

import Tooltip from "./Tooltip";
import {
  StyButton,
  StyInput,
  Switch,
  StySelect,
  StyOption,
} from "./styled/StyledComponents";

import { GGS_API } from "../api_interface/ggs_api";
import { GwsGlobal } from "../GlobalContext";

const EditBoxContainer = styled.form`
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

const EditButtonCont = styled.div`
  display: flex;
  justify-content: center;
`;

const EditableTooltip = styled(Tooltip)`
  width: 100%;
`;

const EditBoxStatus = styled.div`
  display: flex;
  flex-direction: row;
  justify-content: space-between;
  align-items: center;
`;

function Item({ id, json, input, value, values, setValues }) {
  const types = ["string", "int", "float", "bool", "enum"];

  const onChange = (event) => {
    let new_value = event.target.value;
    if (json.type === "bool") {
      new_value = event.target.checked;
    } else if (json.type === "int") {
      new_value = parseInt(new_value);
    } else if (json.type === "float") {
      new_value = parseFloat(new_value);
    }
    setValues({ ...values, [id]: new_value });
  };

  // Check if the item is valid
  if (id === undefined || json.type === undefined || json.name === undefined) {
    return (
      <ItemStyle>
        <span>Invalid item</span>
      </ItemStyle>
    );
  }

  // Check if the item type is valid
  if (!types.includes(json.type)) {
    return (
      <ItemStyle>
        <span>Unknown type: {id}</span>
      </ItemStyle>
    );
  }

  // Check if the item is an enum and has options
  if (json.type === "enum" && json.options === undefined) {
    return (
      <ItemStyle>
        <span>Enum type missing options: {id}</span>
      </ItemStyle>
    );
  }

  // wut is this
  if (input && json.type === "enum") {
  }

  if (value === undefined) {
    if (json.default !== undefined) {
      //value = json.default;
    }
  }

  let display_value = value;
  if (json.type === "bool") {
    if (json.true === undefined) {
      display_value = value ? "true" : "false";
    } else {
      display_value = value ? json.true : json.false;
    }
  }

  // If input is true, then we are in edit mode and need to display the input
  let input_field = null;
  if (json.type === "string") {
    input_field = (
      <StyInput
        type="text"
        pattern={json.pattern}
        defaultValue={value}
        onChange={onChange}
      />
    );
  } else if (json.type === "int") {
    input_field = (
      <StyInput
        type="number"
        step={1}
        min={json.min}
        max={json.max}
        defaultValue={value}
        onChange={onChange}
      />
    );
  } else if (json.type === "float") {
    input_field = (
      <input
        type="number"
        step={json.step}
        min={json.min}
        max={json.max}
        defaultValue={value}
        onChange={onChange}
      />
    );
  } else if (json.type === "bool") {
    input_field = (
      <Switch type="checkbox" defaultChecked={value} onChange={onChange} />
    );
  } else if (json.type === "enum") {
    input_field = (
      <StySelect defaultValue={value} onChange={onChange}>
        {json.options.map((option, i) => {
          return (
            <StyOption key={i} value={option}>
              {option}
            </StyOption>
          );
        })}
      </StySelect>
    );
  }

  return (
    <ItemStyle>
      <ItemName>
        <EditableTooltip text={json.description}>{json.name}</EditableTooltip>
      </ItemName>
      {input ? input_field : <ItemValue>{display_value}</ItemValue>}
    </ItemStyle>
  );
}

export function EditBox({ resource, category }) {
  const { ggsAddress, isGgsConnected } = React.useContext(GwsGlobal);
  // const { ggsConnectionStatus } = React.useContext(GGS_API);

  const [metadata, setMetadata] = React.useState(null);
  const [values, setValues] = React.useState({});
  const [error, setError] = React.useState(null);
  const [editMode, setEditMode] = React.useState(false);

  const encoded_category = encodeURIComponent(category);
  const path = `${ggsAddress}/api/${resource}/settings?category=${encoded_category}&include=all`;

  React.useEffect(() => {
    fetch(path)
      .then((response) => {
        if (!response.ok) {
          throw new Error("Failed to load metadata.");
        } else {
          return response.json();
        }
      })
      .then((data) => {
        if (data.values === undefined) {
          throw new Error("Metadata missing values.");
        }
        if (data.metadata === undefined) {
          throw new Error("Metadata missing meta.");
        }
        setMetadata(data.metadata);
        const new_values = {};
        for (const [key, value] of Object.entries(data.values)) {
          new_values[key] = value;
        }
        setValues(new_values);
      })
      .catch((error) => {
        console.error("Error attempting to load metadata:\n", error);
        setError("Failed to load metadata. (Check console for details.)");
      });
    console.log("EditBox: useEffect");
  }, [editMode, ggsAddress, isGgsConnected]);

  if (!isGgsConnected) {
    return <div>Not connected to GWS.</div>;
  }

  function Save() {
    const data = values;
    console.log("Save", data);
    fetch(path, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(data),
    })
      .then((response) => {
        if (!response.ok) {
          throw new Error("Failed to save metadata.");
        } else {
          setEditMode(false);
        }
      })
      .catch((error) => {});
  }

  function toggleEditMode() {
    if (!editMode) {
      setEditMode(true);
    } else {
      Save();
    }
  }

  //console.log(metadata)

  return (
    <>
      {error != null ? (
        <EditBoxStatus>{error}</EditBoxStatus>
      ) : metadata == null ? (
        <EditBoxStatus>Loading Data...</EditBoxStatus>
      ) : (
        <>
          <EditBoxContainer>
            {Object.entries(metadata).map(([key, value]) => (
              <Item
                key={key}
                json={value}
                input={editMode}
                value={values[key]}
                id={key}
                values={values}
                setValues={setValues}
              />
            ))}
          </EditBoxContainer>
          <EditButtonCont>
            <StyButton onClick={toggleEditMode}>
              {editMode ? "save" : "edit"}
            </StyButton>
          </EditButtonCont>
        </>
      )}
    </>
  );
}

export function GfsEditBox({ category }) {
  const { ggsAddress, isGgsConnected, isGfsTcpConnected } =
    React.useContext(GwsGlobal);
  const [metadata, setMetadata] = React.useState(null);
  const [values, setValues] = React.useState({});
  const [error, setError] = React.useState(null);
  const [editMode, setEditMode] = React.useState(false);
  const [saving, setSaving] = React.useState(false);

  const SAVE_DELAY_MS = 5000;

  const encoded_category = encodeURIComponent(category);
  const path = `${ggsAddress}/api/gfs/settings?category=${encoded_category}&include=all`;

  function loadConfigData() {
    fetch(path)
      .then((response) => {
        if (!response.ok) {
          throw new Error("Failed to load metadata.");
        } else {
          return response.json();
        }
      })
      .then((data) => {
        if (data.values === undefined) {
          throw new Error("Metadata missing values.");
        }
        if (data.metadata === undefined) {
          throw new Error("Metadata missing meta.");
        }
        setMetadata(data.metadata);
        const new_values = {};
        for (const [key, value] of Object.entries(data.values)) {
          new_values[key] = value;
        }
        setValues(new_values);
      })
      .catch((error) => {
        console.error("Error attempting to load metadata:\n", error);
        setError("Failed to load metadata. (Check console for details.)");
      });
  }

  React.useEffect(() => {
    loadConfigData();
    console.log("EditBox: useEffect");
  }, [editMode, ggsAddress, isGgsConnected, path]);

  if (!isGgsConnected) {
    return <div>Not connected to GGS.</div>;
  }
  if (!isGfsTcpConnected) {
    return <div>Not connected to GFS via TCP socket.</div>;
  }
  if (error) {
    return <div>{error}</div>;
  }
  if (saving) {
    return <div>Saving...</div>;
  }

  function Save() {
    const data = values;
    setSaving(true);
    fetch(path, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(data),
    })
      .then((response) => {
        if (!response.ok) {
          throw new Error("Failed to save metadata.");
        } else {
          setEditMode(false);
        }
        console.log("Save complete", response);

        setTimeout(() => {
          console.log("Save complete");
          setSaving(false);
          setEditMode(false);
          setValues({});
          setError(null);
          loadConfigData();
        }, SAVE_DELAY_MS);
      })
      .catch((error) => {
        setError("Failed to save.");
      });
  }

  function toggleEditMode() {
    if (!editMode) {
      setEditMode(true);
    } else {
      Save();
    }
  }

  return (
    <>
      {error != null ? (
        <EditBoxStatus>{error}</EditBoxStatus>
      ) : metadata == null ? (
        <EditBoxStatus>Loading Data...</EditBoxStatus>
      ) : (
        <>
          <EditBoxContainer>
            {Object.entries(metadata).map(([key, value]) => (
              <Item
                key={key}
                json={value}
                input={editMode}
                // value={values[key]}
                id={key}
                values={values}
                setValues={setValues}
              />
            ))}
          </EditBoxContainer>
          <EditButtonCont>
            <StyButton onClick={toggleEditMode}>
              {editMode ? "save" : "edit"}
            </StyButton>
          </EditButtonCont>
        </>
      )}
    </>
  );
}
