import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardBreak,
  CardSectionTitle,
  CollapsibleCardSection,
} from "../../core/PageParts";
import { StyButton } from "../../components/styled/StyledComponents.js";
import { useContext, useState, useEffect } from "react";
import { GwsGlobal } from "../../GlobalContext.js";
import { useApiGetData } from "../../api_interface/ggs_api.js";

function ActionItem({ visible, enabled, name, action }) {
  const { ggsAddress } = useContext(GwsGlobal);

  const doAction = () => {
    let command_string = `cmd/flr/${action}/`;
    console.log("Sending command: ", command_string);

    fetch(ggsAddress + "/api/command", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({
        command_string: command_string,
        send_method: "tcp_socket",
      }),
    })
      .then((response) => response.json())
      .then((response) => {
        console.log("Command sent", response);
      })
      .catch((error) => {
        console.error("Error sending command", error);
      });
  };
  return (
    <StyButton
      style={{ display: visible ? "flex" : "none" }}
      onClick={doAction}
      disabled={!enabled}
    >
      {name}
    </StyButton>
  );
}

function SequenceItem({
  name,
  completed,
  status,
  link = null,
  action_item = null,
}) {
  return (
    <li
      style={{
        display: "flex",
        justifyContent: "space-between",
        padding: "0.5em 0",
        borderBottom: "1px solid #ccc",
      }}
    >
      {link ? (
        <a
          href={link}
          style={{
            textDecoration: "none",
            color: "inherit",
            borderBottom: "1px solid grey",
          }}
        >
          <span>{name}</span>
        </a>
      ) : (
        <span>{name}</span>
      )}
      {action_item ? (
        <ActionItem
          visible={action_item.visible}
          enabled={action_item.enabled}
          name={action_item.name}
          action={action_item.action}
        />
      ) : (
        <span
          style={{
            color: completed ? "green" : "red",
          }}
        >
          {status}
        </span>
      )}
    </li>
  );
}

function FlightSequencer() {
  const {
    data: sequencerMetadataResponse,
    isLoading: isSequencerMetadataLoading,
    error,
  } = useApiGetData("flight_data", "sequencer", "metadata");
  const {
    data: sequenceDataResponse,
    isLoading: isSequenceDataLoading,
    error: sequenceDataError,
  } = useApiGetData("flight_data", "sequencer", "data", 1000);

  const [sequencerMetadata, setSequencerMetadata] = useState(null);

  const { isGgsConnected, isGfsTcpConnected, flightData } =
    useContext(GwsGlobal);
  const [flightPhase, setFlightPhase] = useState("no phase data");

  useEffect(() => {
    if (
      sequencerMetadata === null &&
      sequencerMetadataResponse &&
      typeof sequencerMetadataResponse === "object"
    ) {
      setSequencerMetadata(sequencerMetadataResponse);
    }
  }, [sequencerMetadataResponse, sequencerMetadata]);

  useEffect(() => {
    if (sequenceDataResponse) {
      if (
        sequenceDataResponse.current_phase &&
        typeof sequenceDataResponse.current_phase === "string"
      ) {
        setFlightPhase(sequenceDataResponse.current_phase);
      } else {
        setFlightPhase("unknown");
      }
    }
  }, [sequenceDataResponse]);

  return (
    <Card title="Flight Sequencer">
      <CardSectionTitle>Phase: {flightPhase}</CardSectionTitle>
      <CardBreak />
      {isSequencerMetadataLoading ? (
        <PageContent>Loading sequencer metadata...</PageContent>
      ) : error ? (
        <PageContent>
          Error loading sequencer metadata: {error.message}
        </PageContent>
      ) : sequencerMetadata === null ? (
        <PageContent>No sequencer metadata available.</PageContent>
      ) : (
        Object.keys(sequencerMetadata).map((phase) => (
          <CollapsibleCardSection
            key={phase}
            title={sequencerMetadata[phase].title}
            full_width_when_collapsed={true}
            style={{
              opacity: flightPhase === "pre-launch" ? 1 : 0.5,
            }}
          >
            {Object.keys(sequencerMetadata[phase].steps).map((step) => {
              const stepData = sequencerMetadata[phase].steps[step];
              return (
                <SequenceItem
                  key={step}
                  name={stepData.title}
                  completed={false} // placeholder
                  status={stepData.status || "null"}
                />
              );
            })}
          </CollapsibleCardSection>
        ))
      )}

      <CollapsibleCardSection
        title="Recovery Sequence"
        full_width_when_collapsed={true}
        style={{
          opacity: flightPhase === "recovery" ? 1 : 0.5,
        }}
      >
        <SequenceItem
          name="Reset Flight Phase to Pre-Launch"
          completed={false}
          action_item={{
            visible:
              flightPhase !== "ascent" &&
              flightPhase !== "descent" &&
              flightPhase !== "pre-flight",
            enabled: true,
            name: "Enter Pre-Launch Mode",
            action: "epp",
          }}
        />
      </CollapsibleCardSection>
    </Card>
  );
}

export default FlightSequencer;
