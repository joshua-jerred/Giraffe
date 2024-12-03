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
  const [bitTestStatus, setBitTestStatus] = useState("n/d");
  const [missionClockRunning, setMissionClockRunning] = useState(false);

  const [allowSetLaunchMode, setAllowSetLaunchMode] = useState(false);
  const [allowSetPreLaunchMode, setAllowSetPreLaunchMode] = useState(false);
  const [preLaunchMode, setPreLaunchMode] = useState(false);

  const [userMessage, setUserMessage] = useState("");

  useEffect(() => {
    if (flightData) {
      if (flightData.hasOwnProperty("flight_phase")) {
        setFlightPhase(flightData.flight_phase);
      }
      if (flightData.hasOwnProperty("gfs_time_synced")) {
        setGfsTimeSynced(flightData.gfs_time_synced);
      }
      if (flightData.bit_test_status) {
        setBitTestStatus(flightData.bit_test_status);
      }
      if (flightData.hasOwnProperty("mission_clock_running")) {
        setMissionClockRunning(flightData.mission_clock_running);
      }
    }
  }, [flightData]);

  useEffect(() => {
    if (isGfsTcpConnected) {
      if (flightPhase === "Pre-Launch" && gfsTimeSynced) {
        if (bitTestStatus !== "PASS") {
          setAllowSetLaunchMode(false);
          setUserMessage("Waiting for BIT Pass");
          return;
        }

        if (!missionClockRunning) {
          setAllowSetLaunchMode(false);
          setUserMessage("Waiting for Mission Clock Start");
          return;
        }

        setAllowSetLaunchMode(true);
        setUserMessage("");
      } else {
        setAllowSetLaunchMode(false);
        setPreLaunchMode(false);
      }

      if (flightPhase === "Launch" || flightPhase === "Recovery") {
        setAllowSetPreLaunchMode(true);
      } else {
        setAllowSetPreLaunchMode(false);
      }
    }
  }, [
    isGfsTcpConnected,
    flightPhase,
    gfsTimeSynced,
    bitTestStatus,
    missionClockRunning,
  ]);

  return (
    <>
      <CardSectionTitle>Core Control</CardSectionTitle>
      <CardContentCentered>
        {(flightPhase === "Ascent" || flightPhase === "Descent") && (
          <p>no control options</p>
        )}
        {/* {preLaunchMode &&  */}
        {/* <BitTestPanel /> */}
        {/* } */}
        <p
          style={{
            fontSize: "15px",
          }}
        >
          {userMessage}
        </p>
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
      </CardContentCentered>
    </>
  );
}

export default CoreControl;
