import { useState, useContext, useEffect } from "react";
import styled from "styled-components";
import {
  StyInput,
  Switch,
  StySelect,
  StyOption,
  StyButton,
} from "./styled/StyledComponents";
import { CardBreak } from "../core/PageParts";

import { GwsGlobal } from "../GlobalContext";

import { ConfigMetadata } from "giraffe-protocol";
const ExtensionConfigMetadata = ConfigMetadata.adc_mappings;

const validateExtensionMetadata = (config) => {
  if (config.name.length < ExtensionConfigMetadata.name.min) {
    console.log(config.name.length, ExtensionConfigMetadata.name.min);
    return "Name too short";
  }
  if (config.name.length > ExtensionConfigMetadata.name.max) {
    return "Name too long";
  }
  if (config.update_interval < ExtensionConfigMetadata.update_interval.min) {
    return "Update interval too short";
  }
  if (config.update_interval > ExtensionConfigMetadata.update_interval.max) {
    return "Update interval too long";
  }
  if (!ExtensionConfigMetadata.type.options.includes(config.type)) {
    return "Invalid type";
  }
  if (config.type === "unknown") {
    return "Must select a type";
  }
  if (config.extra_args.length > ExtensionConfigMetadata.extra_args.max) {
    return "Extra args too long";
  }
  return null;
};

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

  .ext_error_text_div {
    text-align: center;
    color: red;
    font-weight: bold;
  }

  .ext_buttons_container {
    display: flex;
    justify-content: space-around;
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
      <div>
        {`${name}[${extensionType}] ${
          enabled ? "Enabled" : "Disabled"
        } ${updateInterval}ms ${critical ? " - Critical" : ""} `}
      </div>
      <div>{extraArgs}</div>

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
        <div className="new_extension_labels">Extension Name</div>
        <StyInput
          type="text"
          defaultValue={name}
          onChange={(e) => setName(e.target.value)}
          className="new_extension_input"
        />

        <div className="new_extension_checkboxes">
          <div className="new_extension_checkbox_container">
            <div className="new_extension_labels">Enabled</div>
            <Switch checked={enabled} setChecked={setEnabled} />
          </div>

          <div className="new_extension_checkbox_container">
            <div className="new_extension_labels">Critical</div>
            <Switch checked={critical} setChecked={setCritical} />
          </div>

          <div className="new_extension_checkbox_container">
            <div className="new_extension_labels">Extension Type</div>
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
        <div className="new_extension_labels">Update Interval</div>
        <StyInput
          className="new_extension_input"
          type="number"
          defaultValue={updateInterval}
          onChange={(e) => setUpdateInterval(parseInt(e.target.value))}
        />
        <div className="new_extension_labels">Extra Args</div>
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

function NewExtension({ addExtension }) {
  const { ggsAddress, isGfsTcpConnected, isGgsConnected } =
    useContext(GwsGlobal);

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
  const [loading, setLoading] = useState(null);
  const [error, setError] = useState(null);

  const submitExtension = () => {
    let config = {
      name: name,
      enabled: enabled,
      type: type,
      update_interval: updateInterval,
      critical: critical,
      extra_args: extraArgs,
    };
    console.log(config);
    let errorMessage = validateExtensionMetadata(config);
    if (errorMessage) {
      setError(errorMessage);
      return;
    }
    setError(null);
    setLoading("submitting...");

    fetch(`${ggsAddress}/api/gfs/extensions`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(config),
    })
      .then((response) => {
        if (!response.ok) {
          setLoading(null);
          setError("Failed to submit extension");
          throw new Error("Failed to submit extension");
        }
        return response.json();
      })
      .then((data) => {
        console.log(data);
        setError(null);
        setLoading(null);
        addExtension(config);

        // setTimeout(() => {
        // fetchExtensions();
        // }, 2000);
      })
      .catch((error) => {
        console.error("Error:", error);
        setLoading(null);
      });
  };

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
      {error && <div className="ext_error_text_div">{error}</div>}
      {!loading ? (
        <StyButton
          onClick={() => submitExtension()}
          className="new_ext_submit_button"
        >
          Submit
        </StyButton>
      ) : (
        <div className="new_ext_submit_button">{loading}</div>
      )}
    </>
  );
}

function ExistingExtension({
  metadata,
  fetchExtensions,
  removeExistingExtension,
}) {
  const { ggsAddress, isGfsTcpConnected, isGgsConnected } =
    useContext(GwsGlobal);

  const [editMode, setEditMode] = useState(false);
  const [loading, setLoading] = useState(null);
  const [error, setError] = useState(null);

  const [name, setName] = useState(metadata.name);
  const [enabled, setEnabled] = useState(metadata.enabled);
  const [type, setType] = useState(metadata.type);
  const [updateInterval, setUpdateInterval] = useState(
    metadata.update_interval
  );
  const [critical, setCritical] = useState(metadata.critical);
  const [extraArgs, setExtraArgs] = useState(metadata.extra_args);

  const editButtonClick = () => {
    if (!editMode) {
      setEditMode(true);
      return;
    }

    let config = {
      name: name,
      enabled: enabled,
      type: type,
      update_interval: updateInterval,
      critical: critical,
      extra_args: extraArgs,
    };
    console.log(config);
    let errorMessage = validateExtensionMetadata(config);
    if (errorMessage) {
      setError(errorMessage);
      return;
    }
    setError(null);
    setLoading("submitting...");

    // use the old name in the query
    fetch(`${ggsAddress}/api/gfs/extensions?extension_name=${metadata.name}`, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(config),
    })
      .then((response) => {
        if (!response.ok) {
          setLoading(null);
          setError("Failed to submit extension");
          throw new Error("Failed to submit extension");
        }
        return response.json();
      })
      .then((data) => {
        console.log(data);
        setError(null);
        setLoading(null);
        setEditMode(false);

        setTimeout(() => {
          fetchExtensions();
        }, 1000);
      })
      .catch((error) => {
        console.error("Error:", error);
        setLoading(null);
      });
  };

  const deleteButtonClick = () => {
    setLoading("deleting...");
    fetch(`${ggsAddress}/api/gfs/extensions?extension_name=${name}`, {
      method: "DELETE",
    })
      .then((response) => {
        if (!response.ok) {
          setLoading(null);
          setError("Failed to delete extension");
          throw new Error("Failed to delete extension");
        }
        return response.json();
      })
      .then((data) => {
        console.log(data);
        // setLoading(null);
        setTimeout(() => {
          // fetchExtensions();
          removeExistingExtension(name);
        }, 3000);
      })
      .catch((error) => {
        console.error("Error:", error);
        setError("Failed to delete extension");
        setLoading(null);
      });
  };

  useEffect(() => {
    if (!error) {
      return;
    }
    setTimeout(() => {
      setError(null);
    }, 3000);
  }, [error]);

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
      {error && <div className="ext_error_text_div">{error}</div>}
      {loading ? (
        <div className="ext_edit_button">{loading}</div>
      ) : (
        <div className="ext_buttons_container">
          <StyButton
            onClick={() => editButtonClick()}
            className="ext_edit_button"
          >
            {editMode ? "Submit" : "Edit"}
          </StyButton>
          <StyButton
            onClick={() => deleteButtonClick()}
            className="ext_edit_button"
          >
            {" "}
            Delete{" "}
          </StyButton>{" "}
        </div>
      )}
    </>
  );
}

function ExtensionsConfiguration() {
  const { ggsAddress, isGfsTcpConnected, isGgsConnected } =
    useContext(GwsGlobal);

  const [addNew, setAddNew] = useState(false);
  const [existingExtensions, setExistingExtensions] = useState([]);
  const [loading, setLoading] = useState(null);
  const [error, setError] = useState(null);

  const fetchExtensions = () => {
    fetch(`${ggsAddress}/api/gfs/settings?category=extensions`)
      .then((response) => {
        if (!response.ok) {
          throw new Error("Failed to fetch extensions");
        }
        return response.json();
      })
      .then((data) => {
        console.log(data);
        setExistingExtensions(data.values);
      })
      .catch((error) => {
        console.error("Error:", error);
      });
  };

  const removeExistingExtension = (name) => {
    let newExtensions = existingExtensions.filter((ext) => ext.name !== name);
    setExistingExtensions(newExtensions);
  };

  const addExtension = (config) => {
    let newExtensions = existingExtensions.slice();
    newExtensions.push(config);
    setExistingExtensions(newExtensions);
  };

  useEffect(() => {
    fetchExtensions();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);

  if (!isGgsConnected) {
    return <div>Not connected to GGS</div>;
  }

  if (!isGfsTcpConnected) {
    return <div>Not connected to GFS</div>;
  }

  return (
    <ExtensionsConfigurationStyle>
      {existingExtensions.map((extension) => (
        <ExistingExtension
          key={extension.name}
          metadata={extension}
          fetchExtensions={fetchExtensions}
          removeExistingExtension={removeExistingExtension}
        />
      ))}
      <CardBreak />
      <StyButton onClick={() => setAddNew(!addNew)} className="new_ext_button">
        {addNew ? "Close" : "New"}
      </StyButton>
      {addNew && <NewExtension addExtension={addExtension} />}
    </ExtensionsConfigurationStyle>
  );
}

function AdcMappingsConfig() {
  return (
    <div>
      <h1>ADC Mappings Configuration</h1>
      <p>ADC Mappings configuration goes here</p>
    </div>
  );
}

export default AdcMappingsConfig;
