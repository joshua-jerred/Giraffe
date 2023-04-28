import React from 'react';
import styled from 'styled-components';

import Tooltip from './Tooltip';

import { create_request_meta } from 'giraffe-protocol/message';

import { GwsGlobal } from '../GlobalContext';

import { validate } from 'giraffe-protocol/message';

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

function Item({ json, value }) {
  if (json.id === undefined || json.name === undefined) {
    return (
      <ItemStyle>
        <span>Invalid item</span>
      </ItemStyle>
    );
  }

  let display_value = value;
  if (json.type === 'bool') {
    if (json.true === undefined) {
      display_value = value ? 'true' : 'false';
    } else {
      display_value = value ? json.true : json.false;
    }
  }

  return (
    <ItemStyle>
      <ItemName>
        <DataTooltip text={json.description}>{json.name}</DataTooltip>
      </ItemName>
      <ItemValue>{display_value}</ItemValue>
    </ItemStyle>
  );
}

export function DataBox({ api, category, subcategory }) {
  const { ggsConnectionStatus, lastJsonMessage, sendJsonMessage } =
    React.useContext(GwsGlobal);

  const [metadata, setMetadata] = React.useState(null);
  const [error, setError] = React.useState(null);
  const [values, setValues] = React.useState(null);

  React.useEffect(() => {
    console.log('DataBox: useEffect');
    if (error !== null) {
      return;
    } else if (ggsConnectionStatus !== 'connected') {
    } else if (metadata === null) {
      let req = create_request_meta('client', api, category, subcategory);
      sendJsonMessage(req);
      if (
        lastJsonMessage !== null &&
        validate(lastJsonMessage) === true &&
        (lastJsonMessage.type === 'meta' || lastJsonMessage.type === 'nack') &&
        lastJsonMessage.body.api === api &&
        lastJsonMessage.body.category === category &&
        (lastJsonMessage.body.subcategory === subcategory ||
          subcategory === undefined)
      ) {
        if (lastJsonMessage.type === 'nack') {
          setError('nack');
          return;
        }
        let body = lastJsonMessage.body;
        if (body.meta !== undefined && body.meta !== null) {
          setMetadata(body);
        }
      }
    } else if (setMetadata) {
    }
  });

  if (ggsConnectionStatus !== 'connected') {
    return <div>Not connected to GWS.</div>;
  }

  return (
    <>
      {error != null ? (
        <DataBoxStatus>{error}</DataBoxStatus>
      ) : metadata === null || metadata === undefined ? (
        <DataBoxStatus>Loading Data...</DataBoxStatus>
      ) : (
        <>
          <DataBoxContainer>
            {metadata.meta.map((item) => (
              <Item
                key={item.id}
                json={item}
                value={''}
                id={item.id}
                values={values}
              />
            ))}
          </DataBoxContainer>
        </>
      )}
    </>
  );
}
