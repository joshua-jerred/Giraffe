import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardBreak,
  CardSectionTitle,
  CollapsibleCardSection,
} from "../../core/PageParts";
import { useContext, useState, useEffect } from "react";
import { GwsGlobal } from "../../GlobalContext.js";

function SequenceItem({ name, completed, link = null }) {
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
      <span
        style={{
          color: completed ? "green" : "red",
        }}
      >
        {completed ? "complete" : "inc"}
      </span>
    </li>
  );
}

function FlightSequencer() {
  const {
    serviceStatuses,
    isGgsConnected,
    isGfsTcpConnected,
    flightData,
    showVerboseClock,
    setShowVerboseClock,
  } = useContext(GwsGlobal);
  const [flightPhase, setFlightPhase] = useState("no phase data");

  useEffect(() => {
    if (flightData && flightData.flight_phase) {
      setFlightPhase(flightData.flight_phase.toLowerCase());
    }
  }, [flightData]);

  return (
    <Card title="Flight Sequencer">
      <CardSectionTitle>Phase: {flightPhase}</CardSectionTitle>
      <CardBreak />
      <CollapsibleCardSection
        title="Pre-Launch Sequence"
        full_width_when_collapsed={true}
        style={{
          opacity: flightPhase === "pre-launch" ? 1 : 0.5,
        }}
      >
        <SequenceItem
          name="Connected to GFS via TCP"
          completed={isGfsTcpConnected}
        />
        <SequenceItem
          name="Configure GFS"
          completed={false}
          link="/gfs/configure"
        />
        <SequenceItem name="Set Launch Position" completed={false} />
        <SequenceItem name="GDL Connected" completed={false} />
        <SequenceItem name="Start Mission Clock" completed={false} />
        <SequenceItem name="Pass BIT test" completed={false} />
        <SequenceItem name="Enter Launch Phase" completed={false} />
      </CollapsibleCardSection>
      <CollapsibleCardSection
        title="Recovery Sequence"
        full_width_when_collapsed={true}
        style={{
          opacity: flightPhase === "recovery" ? 1 : 0.5,
        }}
      >
        <SequenceItem
          name="Connect to GFS via TCP"
          completed={isGfsTcpConnected}
        />
        <SequenceItem
          name="Load All Flight Data"
          completed={false}
          // link="/gfs/configure"
        />
      </CollapsibleCardSection>
    </Card>
  );
}

export default FlightSequencer;
