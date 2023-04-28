import React from 'react';
import styled from 'styled-components';

import Tooltip from './Tooltip';
import { Button, Input, Switch, Select, Option } from './StyledComponents';

import { create_request_meta } from 'giraffe-protocol/message';

import { GwsGlobal } from '../GlobalContext';

import { validate, create_save_values } from 'giraffe-protocol/message';

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

function Item({ json, input, value, values, setValues }) {
  const types = ['string', 'int', 'float', 'bool', 'enum'];

  const onChange = (event) => {
    let new_value = event.target.value;
    if (json.type === 'bool') {
      new_value = event.target.checked;
    } else if (json.type === 'int') {
      new_value = parseInt(new_value);
    } else if (json.type === 'float') {
      new_value = parseFloat(new_value);
    }
    setValues({ ...values, [json.id]: new_value });
  };

  // Check if the item is valid
  if (
    json.id === undefined ||
    json.type === undefined ||
    json.name === undefined
  ) {
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
        <span>Unknown type: {json.id}</span>
      </ItemStyle>
    );
  }

  // Check if the item is an enum and has options
  if (json.type === 'enum' && json.options === undefined) {
    return (
      <ItemStyle>
        <span>Enum type missing options: {json.id}</span>
      </ItemStyle>
    );
  }

  // wut is this
  if (input && json.type === 'enum') {
  }

  if (value === undefined) {
    if (json.default !== undefined) {
      value = json.default;
    }
  }

  let display_value = value;
  if (json.type === 'bool') {
    if (json.true === undefined) {
      display_value = value ? 'true' : 'false';
    } else {
      display_value = value ? json.true : json.false;
    }
  }

  // If input is true, then we are in edit mode and need to display the input
  let input_field = null;
  if (json.type === 'string') {
    input_field = (
      <Input
        type="text"
        pattern={json.pattern}
        defaultValue={value}
        onChange={onChange}
      />
    );
  } else if (json.type === 'int') {
    input_field = (
      <Input
        type="number"
        step={1}
        min={json.min}
        max={json.max}
        defaultValue={value}
        onChange={onChange}
      />
    );
  } else if (json.type === 'float') {
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
  } else if (json.type === 'bool') {
    input_field = (
      <Switch type="checkbox" defaultChecked={value} onChange={onChange} />
    );
  } else if (json.type === 'enum') {
    input_field = (
      <Select defaultValue={value} onChange={onChange}>
        {json.options.map((option, i) => {
          return (
            <Option key={i} value={option}>
              {option}
            </Option>
          );
        })}
      </Select>
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

export function EditBox({ api, category, subcategory }) {
  const { ggsConnectionStatus, sendJsonMessage, lastJsonMessage } =
    React.useContext(GwsGlobal);

  const [metadata, setMetadata] = React.useState(null);
  const [error, setError] = React.useState(null);
  const [editMode, setEditMode] = React.useState(false);
  const [values, setValues] = React.useState(null);

  React.useEffect(() => {
    if (error !== null) {
      return;
    } else if (ggsConnectionStatus !== 'connected') {
      setMetadata(null);
      setValues(null);
      setEditMode(false);
    } else if (metadata === null) {
      let req = create_request_meta('client', api, category, subcategory);
      sendJsonMessage(req);
      if (
        lastJsonMessage !== null &&
        validate(lastJsonMessage) === true &&
        lastJsonMessage.body.category === category
      ) {
        if (
          lastJsonMessage.type === 'nack' &&
          lastJsonMessage.body.category === category
        ) {
          setError('nack');
          return;
        }
        let body = lastJsonMessage.body;
        if (body.meta !== undefined && body.meta !== null) {
          setMetadata(body);
        }
      }
    } else if (values === null) {
      const new_values = {};
      for (const [key, value] of Object.entries(metadata.values)) {
        new_values[key] = value;
        setValues(new_values);
      }
    } else if (editMode) {
      if (lastJsonMessage.type === 'save_ack') {
        let body = lastJsonMessage.body;
        if (body.api === api && body.category === category) {
          setEditMode(false);
          setValues(null);
          setMetadata(null);
          setError(null);
        }
      }
    }
  }, [
    editMode,
    lastJsonMessage,
    metadata,
    api,
    category,
    subcategory,
    values,
    sendJsonMessage,
    ggsConnectionStatus,
    error,
  ]);

  if (ggsConnectionStatus !== 'connected') {
    return <div>Not connected to GWS.</div>;
  }

  function Save() {
    const data = values;

    let save_packet = create_save_values(api, category, subcategory, data);
    sendJsonMessage(save_packet);
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
            {metadata.meta.map((item) => (
              <Item
                key={item.id}
                json={item}
                input={editMode}
                value={metadata.values[item.id]}
                id={item.id}
                values={values}
                setValues={setValues}
              />
            ))}
          </EditBoxContainer>
          <EditButtonCont>
            <Button onClick={toggleEditMode}>
              {editMode ? 'save' : 'edit'}
            </Button>
          </EditButtonCont>
        </>
      )}
    </>
  );
}
