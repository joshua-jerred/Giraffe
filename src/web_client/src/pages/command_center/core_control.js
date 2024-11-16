import { useState, useEffect, useContext } from "react";
import styled, { useTheme } from "styled-components";

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

const TestGroupStyle = styled.div`
  width: 90px;
  height: 90px;
  border-radius: 5px;
  background: ${(props) => props.theme.surface_alt};
  border: 2px solid #6c6b6b;

  display: flex;
  align-items: center;
  flex-direction: column;
  justify-content: center;
  padding: 5px;

  font-size: 23px;
  font-family: ${(props) => props.theme.fonts.mono};
  font-weight: 500;
  font-style: ${(props) => props.theme.fonts.input.style};

  &:hover {
    border: 2px solid #ffa630;
    cursor: pointer;
  }
`;
function TestGroup({ name, status }) {
  const theme = useTheme();

  let name_label = name;
  let status_label = status;
  let style = {
    background: "#6c6b6b",
    opacity: 1,
    color: theme.surface,
  };

  switch (status) {
    case "N/R":
      style.background = "#6c6b6b";
      style.opacity = 10;
      break;
    case "RUN":
      style.background = theme.warning;
      break;
    case "PASS":
      style.background = "rgb(77 191 53)";
      break;
    case "FAIL":
      style.background = theme.error;
      break;
    case "SKIP":
      style.background = "#F7B801";
      style.opacity = 0.4;
      break;
    default:
      style.background = "#6c6b6b";
      status_label = "--";
      style.opacity = 0.4;
      name_label = "INOP";
      break;
  }

  return (
    <TestGroupStyle className="noselect">
      <span>{name_label}</span>
      <div
        style={{
          width: "80%",
          height: "30%",

          display: "flex",
          alignItems: "center",
          justifyContent: "center",
          ...style,
        }}
      >
        <span>{status_label}</span>
      </div>
    </TestGroupStyle>
  );
}

function BitTestPanel() {
  const GroupDataSchema = {
    status: "N/R",
  };
  const [testGroupData, setTestGroupData] = useState({
    FCS: GroupDataSchema,
    SYS: GroupDataSchema,
    DLNK: GroupDataSchema,
    GPS: GroupDataSchema,
    BATT: GroupDataSchema,
    EXTN: GroupDataSchema,
    ENV: GroupDataSchema,
    CAM: GroupDataSchema,
    IMU: GroupDataSchema,
    ADC: GroupDataSchema,
  });

  // Fake bit test data
  useEffect(() => {
    setTestGroupData({
      ...testGroupData,
      FCS: { status: "N/R" },
      SYS: { status: "RUN" },
      DLNK: { status: "PASS" },
      GPS: { status: "FAIL" },
      BATT: { status: "SKIP" },
      EXTN: { status: "inop junk test" },
    });
    // }, 2000);
    // return () => clearInterval(interval);
  }, []);

  return (
    <div
      style={{
        padding: "10px",
      }}
    >
      <CardSectionTitle
        style={{
          justifyContent: "space-between",
        }}
      >
        <span>Pre-Launch BIT</span>
        <StyButton>Run BIT</StyButton>
      </CardSectionTitle>
      <div
        style={{
          width: "100%",
          height: "auto",
          display: "flex",
          alignItems: "start",
          alignContent: "stretch",
          justifyContent: "center",
          gap: "10px",
          padding: "5px 5px 5px 5px",
          flexDirection: "row",
          flexWrap: "wrap",
          flexGrow: "1",
        }}
      >
        {Object.keys(testGroupData).map((key) => {
          return (
            <TestGroup
              key={key}
              name={key}
              status={testGroupData[key].status}
            />
          );
        })}
      </div>
    </div>
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
        <BitTestPanel />
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
