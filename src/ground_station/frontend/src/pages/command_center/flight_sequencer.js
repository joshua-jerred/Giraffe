import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardBreak,
  CardSectionTitle,
  CollapsibleCardSection,
} from "../../core/PageParts";
import {
  StyButton,
  StyNeutralButton,
} from "../../components/styled/StyledComponents.js";
import { useContext, useState, useEffect } from "react";
import { GwsGlobal } from "../../GlobalContext.js";
import { useApiGetData } from "../../api_interface/ggs_api.js";
import Tooltip from "../../components/Tooltip.js";

function SequenceItemUserInput({ user_input, active_step }) {
  const { ggsAddress } = useContext(GwsGlobal);
  if (user_input.type !== "button") {
    console.warn("Unsupported user input type:", user_input.type);
    return <>invalid user input type</>;
  }

  return (
    <StyNeutralButton
      style={{
        opacity: active_step ? 1 : 0.5,
        cursor: active_step ? "pointer" : "not-allowed",
      }}
      onClick={() => {
        if (!active_step) {
          console.warn("Cannot perform action, step is not active");
          return;
        }

        fetch(ggsAddress + "/api/flight_data/sequencer", {
          method: "POST",
          headers: {
            "Content-Type": "application/json",
          },
          body: JSON.stringify({
            step_label: user_input.action,
          }),
        })
          .then((response) => response.json())
          .then((response) => {
            console.log("User input action performed:", response);
          })
          .catch((error) => {
            console.error("Error performing user input action:", error);
          });
      }}
    >
      {user_input.label}
    </StyNeutralButton>
  );
}

function SequenceItem({
  name,
  completed,
  status,
  tooltip,
  link = null,
  active_step = false,
  user_input = null,
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
      <Tooltip text={tooltip}>
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
          <span
            style={{
              opacity: completed ? 0.5 : 1,
            }}
          >
            {name}
          </span>
        )}
      </Tooltip>
      {active_step && user_input ? (
        <SequenceItemUserInput
          user_input={user_input}
          active_step={active_step}
        />
      ) : (
        <span
          style={{
            color: completed ? "green" : "grey",
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
  const [sequenceData, setSequenceData] = useState(null);
  const [flightPhase, setFlightPhase] = useState("no phase data");
  const [activeStep, setActiveStep] = useState(null);

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
      if (
        sequenceDataResponse.sequence &&
        typeof sequenceDataResponse.sequence === "object"
      ) {
        setSequenceData(sequenceDataResponse.sequence);
      } else {
        setSequenceData(null);
      }
      if (
        sequenceDataResponse.current_step &&
        typeof sequenceDataResponse.current_step === "string"
      ) {
        setActiveStep(sequenceDataResponse.current_step);
      } else {
        setActiveStep(null);
      }
    }
  }, [sequenceDataResponse]);

  return (
    <Card title="Flight Sequencer">
      {/* <CardSectionTitle>Phase: {flightPhase}</CardSectionTitle> */}
      {/* <CardBreak /> */}
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
              opacity: flightPhase === phase ? 1 : 0.5,
            }}
          >
            {Object.keys(sequencerMetadata[phase].steps).map((step) => {
              const stepMetadata = sequencerMetadata[phase].steps[step];
              const stepData =
                typeof sequenceData === "object" &&
                sequenceData.hasOwnProperty(step)
                  ? sequenceData[step]
                  : {
                      completed: false,
                      status: "err data",
                    };

              return (
                <SequenceItem
                  key={step}
                  name={stepMetadata.title}
                  tooltip={stepMetadata.description || ""}
                  completed={stepData.completed}
                  status={stepData.status || "null"}
                  active_step={activeStep === step}
                  user_input={stepMetadata.user_input || null}
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
          // action_item={{
          //   visible:
          //     flightPhase !== "ascent" &&
          //     flightPhase !== "descent" &&
          //     flightPhase !== "pre-flight",
          //   enabled: true,
          //   name: "Enter Pre-Launch Mode",
          //   action: "epp",
          // }}
        />
      </CollapsibleCardSection>
    </Card>
  );
}

export default FlightSequencer;
