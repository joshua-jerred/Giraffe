import { StySaveButton, StyButton, StyInput } from "./styled/StyledComponents";
import { useState, useEffect, useContext } from "react";
import styled from "styled-components";

import { GwsGlobal } from "../GlobalContext";
import Tooltip from "./Tooltip";

const ItemEditContainer = styled.div`
  margin: 0.2em 0;
`;

const ItemEditTitle = styled.div``;

const ItemSaveButton = styled(StySaveButton)`
  max-width: 5em;
`;

const ItemEditButton = styled(StyButton)`
  max-width: 5em;
`;

const ItemEditInput = styled(StyInput)`
  width: 98%;
  min-width: 10em;
`;

const ItemEditForm = styled.div`
  display: flex;
  flex-direction: row;
  align-items: center;
  justify-content: space-between;
  & > ${StyButton} {
    margin-right: 0.2em;
    margin-left: 0.2em;
  }
  & > ${ItemSaveButton} {
    margin-left: 1em;
  }
`;

export function LocalItemEdit({ title, state, setState, tooltip, onTest }) {
  const [value, setValue] = useState(state);
  const [saved, setSaved] = useState("yes");

  useEffect(() => {
    setSaved(value === state ? "yes" : "no");
  }, [value, state]);

  return (
    <ItemEditContainer>
      <ItemEditTitle>{title}</ItemEditTitle>
      <ItemEditForm>
        <Tooltip text={tooltip} style={{ width: "98%" }}>
          <ItemEditInput
            value={value}
            onChange={(e) => {
              setValue(e.target.value);
            }}
          />
        </Tooltip>
        <ItemSaveButton onClick={() => setState(value)} saved={saved}>
          save
        </ItemSaveButton>
        <ItemEditButton onClick={() => onTest(value)}>test</ItemEditButton>
      </ItemEditForm>
    </ItemEditContainer>
  );
}

// [GET/PUT] /{resource}/{item} -> {value: string}
export function RemoteItemEdit({ title, tooltip, resource, item }) {
  const [localValue, setLocalValue] = useState("unknown");
  const [remoteValue, setRemoteValue] = useState("unknown");
  const [saved, setSaved] = useState("yes");
  const { GwsAddress, GwsConnectionStatus } = useContext(GwsGlobal);

  useEffect(() => {
    setSaved(localValue === remoteValue ? "yes" : "no");
  }, [localValue, remoteValue]);

  if (GwsConnectionStatus !== "connected") {
    return <div>Not connected to GWS</div>;
  }

  function saveValue() {
    console.log("saveValue" + localValue);
  }

  function loadValue() {
    fetch(`${GwsAddress}/${resource}/${item}`)
      .then((response) => response.json())
      .then((data) => {
        setRemoteValue(data.value);
        //setLocalValue(data.value);
      })
      .catch((error) => {
        console.log(error);
      });
  }

  loadValue();

  return (
    <ItemEditContainer>
      <ItemEditTitle>{title}</ItemEditTitle>
      <ItemEditForm>
        <Tooltip text={tooltip} style={{ width: "98%" }}>
          <ItemEditInput
            value={localValue}
            onChange={(e) => {
              setLocalValue(e.target.value);
            }}
          />
        </Tooltip>
        <ItemSaveButton onClick={() => saveValue()} saved={saved}>
          save
        </ItemSaveButton>
      </ItemEditForm>
    </ItemEditContainer>
  );
}
