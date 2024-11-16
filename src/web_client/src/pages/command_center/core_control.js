import { useState, useEffect, useContext } from "react";

import { GwsGlobal } from "../../GlobalContext";
import { StyButton } from "../../components/styled/StyledComponents";
import { CardSectionTitle, CardContentCentered } from "../../core/PageParts";

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
    <li style={{ display: visible ? "flex" : "none" }}>
      <StyButton onClick={doAction} disabled={!enabled}>
        {name}
      </StyButton>
    </li>
  );
}

function CoreControl() {
  const { flightData, isGfsTcpConnected } = useContext(GwsGlobal);
  const [flightPhase, setFlightPhase] = useState("");
  const [gfsTimeSynced, setGfsTimeSynced] = useState(false);

  const [allowSetLaunchMode, setAllowSetLaunchMode] = useState(false);
  const [allowSetPreLaunchMode, setAllowSetPreLaunchMode] = useState(false);
  const [preLaunchMode, setPreLaunchMode] = useState(false);

  useEffect(() => {
    if (flightData) {
      if (flightData.flight_phase) {
        setFlightPhase(flightData.flight_phase);
      }
      if (flightData.gfs_time_synced) {
        setGfsTimeSynced(flightData.gfs_time_synced);
      }
    }
  }, [flightData]);

  useEffect(() => {
    if (isGfsTcpConnected) {
      if (flightPhase === "Pre-Launch" && gfsTimeSynced) {
        setPreLaunchMode(true);
      } else {
        setAllowSetLaunchMode(false);
        setPreLaunchMode(false);
      }

      if (flightPhase === "Launch") {
        setAllowSetPreLaunchMode(true);
      } else {
        setAllowSetPreLaunchMode(false);
      }

      if (flightPhase === "Recovery") {
        setAllowSetPreLaunchMode(true);
      } else {
        setAllowSetPreLaunchMode(false);
      }
    }
  }, [isGfsTcpConnected, flightPhase, gfsTimeSynced]);

  return (
    <>
      <CardSectionTitle>Core Control</CardSectionTitle>
      <CardContentCentered>
        {/* {preLaunchMode &&  */}
        {/* <BitTestPanel /> */}
        {/* } */}
        <ActionItem
          visible={flightPhase === "Pre-Launch"}
          enabled={allowSetLaunchMode}
          name="Activate Launch Mode"
          action="elp"
        />
        <ActionItem
          visible={
            flightPhase !== "Ascent" &&
            flightPhase !== "Descent" &&
            flightPhase !== "Pre-Launch"
          }
          enabled={allowSetPreLaunchMode}
          name="Enter Pre-Launch Mode"
          action="epp"
        />
        <p
          style={{
            fontSize: "12px",
          }}
        >
          Flight Phase: {flightPhase}
          <br />
          Allow Set Launch Mode: {allowSetLaunchMode.toString()}
          <br />
          Allow Set Pre-Launch Mode: {allowSetPreLaunchMode.toString()}
        </p>
      </CardContentCentered>
    </>
  );
}

export default CoreControl;
