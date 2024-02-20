import { useState } from "react";
import styled from "styled-components";
import {
  StyInput,
  Switch,
  StySelect,
  StyOption,
  StyButton,
} from "./styled/StyledComponents";
import { CardBreak } from "../core/PageParts";

import { ConfigMetadata } from "giraffe-protocol";
const ExtensionConfigMetadata = ConfigMetadata.extensions;

const TemporaryData = [
  {
    name: "bme280",
    enabled: true,
    type: "bme280",
    update_interval: 1000,
    critical: false,
    extra_args: "",
  },
  {
    name: "samm8q",
    enabled: true,
    type: "sam_m8q",
    update_interval: 1000,
    critical: false,
    extra_args: "",
  },
];
const ExtensionsConfigurationStyle = styled.div`
  width: 100%;

  .new_ext_button {
    display: block;
    margin: 0 auto;
    margin-bottom: 15px;
    width: 100px;
  }

  .new_ext_submit_button {
    display: block;
    margin: 10px auto;
    margin-bottom: 15px;
    width: 200px;
  }

  .ext_edit_button {
    display: block;
    margin: 10px auto;
    margin-bottom: 15px;
    width: 100px;
  }

  .new_extension_div_container {
    max-width: 450px;
    margin: 0 auto;
    padding: 0px;
  }

  .new_extension_div {
  }

  .new_extension_labels {
    margin-top: 7px;
    font-size: 1em;
    font-weight: bold;
  }

  .new_extension_input {
    margin: 0 3%;
    width: calc(94% - 10px);
    padding: 0px 5px;
    height: 30px;
  }

  .new_extension_checkboxes {
    display: flex;
    justify-content: space-around;
  }

  .new_extension_checkbox_container {
    display: block;
    align-items: center;
    text-align: center;
  }
`;

function ExtensionDisplay({
  name,
  enabled,
  extensionType,
  updateInterval,
  critical,
  extraArgs,
}) {
  return (
    <div>
      <h3>{name}</h3>
      {/* <GfsEditBox resource="gfs" category="extensions" /> */}
    </div>
  );
}

function ExtensionEdit({
  name,
  setName,
  enabled,
  setEnabled,
  extensionType,
  setExtensionType,
  updateInterval,
  setUpdateInterval,
  critical,
  setCritical,
  extraArgs,
  setExtraArgs,
}) {
  return (
    <div className="new_extension_div_container">
      <div className="new_extension_div">
        <div class="new_extension_labels">Extension Name</div>
        <StyInput
          type="text"
          defaultValue={name}
          onChange={(e) => setName(e.target.value)}
          className="new_extension_input"
        />

        <div className="new_extension_checkboxes">
          <div className="new_extension_checkbox_container">
            <div class="new_extension_labels">Enabled</div>
            <Switch checked={enabled} setChecked={setEnabled} />
          </div>

          <div className="new_extension_checkbox_container">
            <div class="new_extension_labels">Critical</div>
            <Switch checked={critical} setChecked={setCritical} />
          </div>

          <div className="new_extension_checkbox_container">
            <div class="new_extension_labels">Extension Type</div>
            <StySelect
              className="new_extension_input"
              defaultValue={extensionType}
              onChange={(e) => setExtensionType(e.target.value)}
            >
              {ExtensionConfigMetadata.type.options.map((option) => (
                <StyOption key={option} value={option}>
                  {option}
                </StyOption>
              ))}
            </StySelect>
          </div>
        </div>
        <div class="new_extension_labels">Update Interval</div>
        <StyInput
          className="new_extension_input"
          type="number"
          defaultValue={updateInterval}
          onChange={(e) => setUpdateInterval(e.target.value)}
        />
        <div class="new_extension_labels">Extra Args</div>
        <StyInput
          className="new_extension_input"
          type="text"
          defaultValue={extraArgs}
          onChange={(e) => setExtraArgs(e.target.value)}
        />
      </div>
    </div>
  );
}

function NewExtension() {
  const [name, setName] = useState(ExtensionConfigMetadata.name.default);
  const [enabled, setEnabled] = useState(
    ExtensionConfigMetadata.enabled.default
  );
  const [type, setType] = useState(ExtensionConfigMetadata.type.default);
  const [updateInterval, setUpdateInterval] = useState(
    ExtensionConfigMetadata.update_interval.default
  );
  const [critical, setCritical] = useState(
    ExtensionConfigMetadata.critical.default
  );
  const [extraArgs, setExtraArgs] = useState(
    ExtensionConfigMetadata.extra_args.default
  );

  const submitExtension = () => {};

  return (
    <>
      <ExtensionEdit
        name={name}
        setName={setName}
        enabled={enabled}
        setEnabled={setEnabled}
        extensionType={type}
        setExtensionType={setType}
        updateInterval={updateInterval}
        setUpdateInterval={setUpdateInterval}
        critical={critical}
        setCritical={setCritical}
        extraArgs={extraArgs}
        setExtraArgs={setExtraArgs}
      />
      <StyButton
        onClick={() => submitExtension}
        className="new_ext_submit_button"
      >
        Submit
      </StyButton>
    </>
  );
}

function ExistingExtension({ metadata }) {
  const [editMode, setEditMode] = useState(false);

  const [name, setName] = useState(metadata.name);
  const [enabled, setEnabled] = useState(metadata.enabled);
  const [type, setType] = useState(metadata.type);
  const [updateInterval, setUpdateInterval] = useState(
    metadata.update_interval
  );
  const [critical, setCritical] = useState(metadata.critical);
  const [extraArgs, setExtraArgs] = useState(metadata.extra_args);

  return (
    <>
      {!editMode ? (
        <ExtensionDisplay
          name={name}
          enabled={enabled}
          extensionType={type}
          updateInterval={updateInterval}
          critical={critical}
          extraArgs={extraArgs}
        />
      ) : (
        <ExtensionEdit
          name={name}
          setName={setName}
          enabled={enabled}
          setEnabled={setEnabled}
          extensionType={type}
          setExtensionType={setType}
          updateInterval={updateInterval}
          setUpdateInterval={setUpdateInterval}
          critical={critical}
          setCritical={setCritical}
          extraArgs={extraArgs}
          setExtraArgs={setExtraArgs}
        />
      )}
      <StyButton onClick={() => setEditMode(true)} className="ext_edit_button">
        Edit
      </StyButton>
    </>
  );
}

function ExtensionsConfiguration() {
  const [addNew, setAddNew] = useState(false);
  return (
    <ExtensionsConfigurationStyle>
      {TemporaryData.map((extension) => (
        <ExistingExtension key={extension.name} metadata={extension} />
      ))}
      <CardBreak />
      <StyButton onClick={() => setAddNew(!addNew)} className="new_ext_button">
        {addNew ? "Close" : "New"}
      </StyButton>
      {addNew && <NewExtension />}
    </ExtensionsConfigurationStyle>
  );
}

export default ExtensionsConfiguration;
