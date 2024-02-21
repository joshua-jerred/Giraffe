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
const AdcMappingConfigMetadata = ConfigMetadata.adc_mappings;

/**
 * @todo Need to complete validation
 * @param {*} config
 * @returns
 */
const validateAdcMappingConfig = (config) => {
  if (config.label.length < AdcMappingConfigMetadata.label.min) {
    console.log(config.label.length, AdcMappingConfigMetadata.label.min);
    return "Label too short";
  }
  if (!AdcMappingConfigMetadata.type.options.includes(config.type)) {
    return "Invalid type";
  }
  if (config.type === "unknown") {
    return "Must select a type";
  }
  return null;
};

const AdcValueMapConfigurationStyle = styled.div`
  width: 100%;

  .new_ext_button {
    display: block;
    margin: 0 auto;
    margin-bottom: 15px;
    width: 100px;
  }

  .new_adc_map_submit_button {
    display: block;
    margin: 10px auto;
    margin-bottom: 15px;
    width: 200px;
  }

  .adc_map_edit_button {
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

  .new_adc_input {
    margin: 0 3%;
    width: calc(94% - 10px);
    padding: 0px 5px;
    height: 30px;
  }

  .new_extension_checkbox_container {
    display: block;
    align-items: center;
    text-align: center;
  }

  .adc_map_error_text_div {
    text-align: center;
    color: red;
    font-weight: bold;
  }

  .adc_map_buttons_container {
    display: flex;
    justify-content: space-around;
  }
`;

function AdcMapDisplay({
  label,
  mapType,
  extensionName,
  channelNumber,
  resolution,
  resistor1Value,
  resistor2Value,
  voltageReference,
  percentageMax,
  percentageMin,
  percentageClamp,
}) {
  return (
    <div>
      <div>
        {`${label} - ${mapType} [${extensionName}:${channelNumber}] - ${resolution}bit`}
      </div>
      {mapType === "percentage" &&
        `${percentageMin}% - ${percentageMax}% - ${percentageClamp}%`}
      {mapType === "voltage_divider" &&
        `${resistor1Value} - ${resistor2Value} - ${voltageReference}mv`}
    </div>
  );
}

function AdcMapEdit({
  label,
  setLabel,
  mapType,
  setMapType,
  extensionName,
  setExtensionName,
  channelNumber,
  setChannelNumber,
  resolution,
  setResolution,
  resistor1Value,
  setResistor1Value,
  resistor2Value,
  setResistor2Value,
  voltageReference,
  setVoltageReference,
  percentageMax,
  setPercentageMax,
  percentageMin,
  setPercentageMin,
  percentageClamp,
  setPercentageClamp,
}) {
  return (
    <div className="new_extension_div_container">
      <div className="new_extension_div">
        <div className="new_extension_labels">Value Label</div>
        <StyInput
          type="text"
          defaultValue={label}
          onChange={(e) => setLabel(e.target.value)}
          className="new_adc_input"
        />

        <div className="new_extension_labels">Value Map Type</div>
        <StySelect
          className="new_adc_input"
          defaultValue={mapType}
          onChange={(e) => setMapType(e.target.value)}
        >
          {AdcMappingConfigMetadata.type.options.map((option) => (
            <StyOption key={option} value={option}>
              {option}
            </StyOption>
          ))}
        </StySelect>

        <div className="new_extension_labels">Extension Name</div>
        <StyInput
          type="text"
          defaultValue={extensionName}
          onChange={(e) => setExtensionName(e.target.value)}
          className="new_adc_input"
        />

        <div className="new_extension_labels">Channel Number</div>
        <StyInput
          className="new_adc_input"
          type="number"
          defaultValue={channelNumber}
          onChange={(e) => setChannelNumber(parseInt(e.target.value))}
        />

        <div className="new_extension_labels">Resolution</div>
        <StyInput
          className="new_adc_input"
          type="number"
          defaultValue={resolution}
          onChange={(e) => setResolution(parseInt(e.target.value))}
        />
        {mapType === "voltage_divider" && (
          <>
            <div className="new_extension_labels">Resistor 1 Value</div>
            <StyInput
              className="new_adc_input"
              type="number"
              defaultValue={resistor1Value}
              onChange={(e) => setResistor1Value(parseInt(e.target.value))}
            />

            <div className="new_extension_labels">Resistor 2 Value</div>
            <StyInput
              className="new_adc_input"
              type="number"
              defaultValue={resistor2Value}
              onChange={(e) => setResistor2Value(parseInt(e.target.value))}
            />

            <div className="new_extension_labels">Voltage Reference</div>
            <StyInput
              className="new_adc_input"
              type="number"
              defaultValue={voltageReference}
              onChange={(e) => setVoltageReference(parseInt(e.target.value))}
            />
          </>
        )}

        {mapType === "percentage" && (
          <>
            <div className="new_extension_labels">Percentage 100% Value</div>
            <StyInput
              className="new_adc_input"
              type="number"
              defaultValue={percentageMax}
              onChange={(e) => setPercentageMax(parseInt(e.target.value))}
            />

            <div className="new_extension_labels">Percentage 0% Value</div>
            <StyInput
              className="new_adc_input"
              type="number"
              defaultValue={percentageMin}
              onChange={(e) => setPercentageMin(parseInt(e.target.value))}
            />

            <div className="new_extension_checkbox_container">
              <div className="new_extension_labels">Percentage Clamp</div>
              <Switch
                checked={percentageClamp}
                setChecked={setPercentageClamp}
              />
            </div>
          </>
        )}
      </div>
    </div>
  );
}

function NewAdcValueMap({ addAdcValueMap }) {
  const { ggsAddress, isGfsTcpConnected, isGgsConnected } =
    useContext(GwsGlobal);

  const [label, setLabel] = useState(AdcMappingConfigMetadata.label.default);
  const [type, setType] = useState(AdcMappingConfigMetadata.type.default);
  const [extensionName, setExtensionName] = useState(
    AdcMappingConfigMetadata.extension_name.default
  );
  const [channelNumber, setChannelNumber] = useState(
    AdcMappingConfigMetadata.channel_number.default
  );
  const [resolution, setResolution] = useState(
    AdcMappingConfigMetadata.resolution.default
  );
  const [resistor1Value, setResistor1Value] = useState(
    AdcMappingConfigMetadata.resistor_1_value.default
  );
  const [resistor2Value, setResistor2Value] = useState(
    AdcMappingConfigMetadata.resistor_2_value.default
  );
  const [voltageReference, setVoltageReference] = useState(
    AdcMappingConfigMetadata.voltage_reference.default
  );
  const [percentageMax, setPercentageMax] = useState(
    AdcMappingConfigMetadata.percentage_max.default
  );
  const [percentageMin, setPercentageMin] = useState(
    AdcMappingConfigMetadata.percentage_min.default
  );
  const [percentageClamp, setPercentageClamp] = useState(
    AdcMappingConfigMetadata.percentage_clamp.default
  );

  const [loading, setLoading] = useState(null);
  const [error, setError] = useState(null);

  const submitAdcMap = () => {
    let config = {
      label: label,
      type: type,
      extension_name: extensionName,
      channel_number: channelNumber,
      resolution: resolution,
      resistor_1_value: resistor1Value,
      resistor_2_value: resistor2Value,
      voltage_reference: voltageReference,
      percentage_max: percentageMax,
      percentage_min: percentageMin,
      percentage_clamp: percentageClamp,
    };
    console.log(config);
    let errorMessage = validateAdcMappingConfig(config);
    if (errorMessage) {
      setError(errorMessage);
      return;
    }
    setError(null);
    setLoading("submitting...");

    fetch(`${ggsAddress}/api/gfs/adc_mappings`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(config),
    })
      .then((response) => {
        if (!response.ok) {
          setLoading(null);
          setError("Failed to submit adc value map");
          throw new Error("Failed to submit adc value map");
        }
        return response.json();
      })
      .then((data) => {
        console.log(data);
        setError(null);
        setLoading(null);
        addAdcValueMap(config);

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
      <AdcMapEdit
        label={label}
        setLabel={setLabel}
        mapType={type}
        setMapType={setType}
        extensionName={extensionName}
        setExtensionName={setExtensionName}
        channelNumber={channelNumber}
        setChannelNumber={setChannelNumber}
        resolution={resolution}
        setResolution={setResolution}
        resistor1Value={resistor1Value}
        setResistor1Value={setResistor1Value}
        resistor2Value={resistor2Value}
        setResistor2Value={setResistor2Value}
        voltageReference={voltageReference}
        setVoltageReference={setVoltageReference}
        percentageMax={percentageMax}
        setPercentageMax={setPercentageMax}
        percentageMin={percentageMin}
        setPercentageMin={setPercentageMin}
        percentageClamp={percentageClamp}
        setPercentageClamp={setPercentageClamp}
      />
      {error && <div className="adc_map_error_text_div">{error}</div>}
      {!loading ? (
        <StyButton
          onClick={() => submitAdcMap()}
          className="new_adc_map_submit_button"
        >
          Submit
        </StyButton>
      ) : (
        <div className="new_adc_map_submit_button">{loading}</div>
      )}
    </>
  );
}

function ExistingAdcValueMap({
  values,
  fetchAdcValueMaps,
  removeExistingAdcValueMap,
}) {
  const { ggsAddress, isGfsTcpConnected, isGgsConnected } =
    useContext(GwsGlobal);

  const [editMode, setEditMode] = useState(false);
  const [loading, setLoading] = useState(null);
  const [error, setError] = useState(null);

  const [label, setLabel] = useState(values.label);
  const [type, setType] = useState(values.type);
  const [extensionName, setExtensionName] = useState(values.extension_name);
  const [channelNumber, setChannelNumber] = useState(values.channel_number);
  const [resolution, setResolution] = useState(values.resolution);
  const [resistor1Value, setResistor1Value] = useState(values.resistor_1_value);
  const [resistor2Value, setResistor2Value] = useState(values.resistor_2_value);
  const [voltageReference, setVoltageReference] = useState(
    values.voltage_reference
  );
  const [percentageMax, setPercentageMax] = useState(values.percentage_max);
  const [percentageMin, setPercentageMin] = useState(values.percentage_min);
  const [percentageClamp, setPercentageClamp] = useState(
    values.percentage_clamp
  );

  const editButtonClick = () => {
    if (!editMode) {
      setEditMode(true);
      return;
    }

    let config = {
      label: label,
      type: type,
      extension_name: extensionName,
      channel_number: channelNumber,
      resolution: resolution,
      resistor_1_value: resistor1Value,
      resistor_2_value: resistor2Value,
      voltage_reference: voltageReference,
      percentage_max: percentageMax,
      percentage_min: percentageMin,
      percentage_clamp: percentageClamp,
    };
    console.log(config);
    let errorMessage = validateAdcMappingConfig(config);
    if (errorMessage) {
      setError(errorMessage);
      return;
    }
    setError(null);
    setLoading("submitting...");

    // use the old name in the query
    fetch(`${ggsAddress}/api/gfs/adc_mappings?map_label=${values.label}`, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(config),
    })
      .then((response) => {
        if (!response.ok) {
          setLoading(null);
          setError("Failed to submit adc value map");
          throw new Error("Failed to submit adc value map");
        }
        return response.json();
      })
      .then((data) => {
        console.log(data);
        setError(null);
        setLoading(null);
        setEditMode(false);

        setTimeout(() => {
          fetchAdcValueMaps();
        }, 1000);
      })
      .catch((error) => {
        console.error("Error:", error);
        setLoading(null);
      });
  };

  const deleteButtonClick = () => {
    setLoading("deleting...");
    fetch(`${ggsAddress}/api/gfs/adc_mappings?map_label=${label}`, {
      method: "DELETE",
    })
      .then((response) => {
        if (!response.ok) {
          setLoading(null);
          setError("Failed to delete adc value map");
          throw new Error("Failed to delete adc value map");
        }
        return response.json();
      })
      .then((data) => {
        console.log(data);
        // setLoading(null);
        setTimeout(() => {
          // fetchExtensions();
          removeExistingAdcValueMap(label);
        }, 3000);
      })
      .catch((error) => {
        console.error("Error:", error);
        setError("Failed to delete adc value map");
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
        <AdcMapDisplay
          label={label}
          mapType={type}
          extensionName={extensionName}
          channelNumber={channelNumber}
          resolution={resolution}
          resistor1Value={resistor1Value}
          resistor2Value={resistor2Value}
          voltageReference={voltageReference}
          percentageMax={percentageMax}
          percentageMin={percentageMin}
          percentageClamp={percentageClamp}
        />
      ) : (
        <AdcMapEdit
          label={label}
          setLabel={setLabel}
          mapType={type}
          setMapType={setType}
          extensionName={extensionName}
          setExtensionName={setExtensionName}
          channelNumber={channelNumber}
          setChannelNumber={setChannelNumber}
          resolution={resolution}
          setResolution={setResolution}
          resistor1Value={resistor1Value}
          setResistor1Value={setResistor1Value}
          resistor2Value={resistor2Value}
          setResistor2Value={setResistor2Value}
          voltageReference={voltageReference}
          setVoltageReference={setVoltageReference}
          percentageMax={percentageMax}
          setPercentageMax={setPercentageMax}
          percentageMin={percentageMin}
          setPercentageMin={setPercentageMin}
          percentageClamp={percentageClamp}
          setPercentageClamp={setPercentageClamp}
        />
      )}
      {error && <div className="adc_map_error_text_div">{error}</div>}
      {loading ? (
        <div className="adc_map_edit_button">{loading}</div>
      ) : (
        <div className="adc_map_buttons_container">
          <StyButton
            onClick={() => editButtonClick()}
            className="adc_map_edit_button"
          >
            {editMode ? "Submit" : "Edit"}
          </StyButton>
          <StyButton
            onClick={() => deleteButtonClick()}
            className="adc_map_edit_button"
          >
            {" "}
            Delete{" "}
          </StyButton>{" "}
        </div>
      )}
    </>
  );
}

function AdcMappingsConfig() {
  const { ggsAddress, isGfsTcpConnected, isGgsConnected } =
    useContext(GwsGlobal);

  const [addNew, setAddNew] = useState(false);
  const [existingAdcMappings, setExistingAdcValueMaps] = useState([]);
  const [loading, setLoading] = useState(null);
  const [error, setError] = useState(null);

  const fetchAdcValueMaps = () => {
    fetch(`${ggsAddress}/api/gfs/settings?category=adc_mappings`)
      .then((response) => {
        if (!response.ok) {
          throw new Error("Failed to fetch adc value maps");
        }
        return response.json();
      })
      .then((data) => {
        console.log(data);
        setExistingAdcValueMaps(data.values);
      })
      .catch((error) => {
        console.error("Error:", error);
      });
  };

  const removeExistingAdcValueMap = (label) => {
    let newAdcValueMaps = existingAdcMappings.filter(
      (adc_value_map) => adc_value_map.label !== label
    );
    setExistingAdcValueMaps(newAdcValueMaps);
  };

  const addAdcValueMap = (config) => {
    let newValueMaps = existingAdcMappings.slice();
    newValueMaps.push(config);
    setExistingAdcValueMaps(newValueMaps);
  };

  useEffect(() => {
    fetchAdcValueMaps();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);

  if (!isGgsConnected) {
    return <div>Not connected to GGS</div>;
  }

  if (!isGfsTcpConnected) {
    return <div>Not connected to GFS</div>;
  }

  return (
    <AdcValueMapConfigurationStyle>
      {existingAdcMappings.map((adc_value_map) => (
        <ExistingAdcValueMap
          key={adc_value_map.label}
          values={adc_value_map}
          fetchAdcValueMaps={fetchAdcValueMaps}
          removeExistingAdcValueMap={removeExistingAdcValueMap}
        />
      ))}
      <CardBreak />
      <StyButton onClick={() => setAddNew(!addNew)} className="new_ext_button">
        {addNew ? "Close" : "New"}
      </StyButton>
      {addNew && <NewAdcValueMap addAdcValueMap={addAdcValueMap} />}
    </AdcValueMapConfigurationStyle>
  );
}

export default AdcMappingsConfig;
