import React, { useState, useEffect, useContext } from "react";
import styled, { useTheme } from "styled-components";

import { GwsGlobal } from "../../GlobalContext";
import { StyButton } from "../../components/styled/StyledComponents";
import { CardSectionTitle } from "../../core/PageParts";
import Tooltip from "../../components/Tooltip";
import { useApiGetData } from "../../api_interface/ggs_api";

const getStatusColor = (status) => {
  const StatusColor = {
    "N/R": "#6c6b6b",
    RUN: "#ffeaa7",
    PASS: "rgb(77 191 53)",
    FAIL: "#f55d3e",
    SKIP: "#F7B801",
  };

  if (status in StatusColor) {
    return StatusColor[status];
  } else {
    return "#6c6b6b";
  }
};

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

  transition: 250ms;
  &:hover {
    border: 2px solid #ffa630;
    cursor: pointer;
  }
`;

function TestGroup({ name, status, selectedGroup, setSelectedGroup }) {
  const { isGfsTcpConnected } = useContext(GwsGlobal);
  const theme = useTheme();

  // console.log("ai name: ", name, status, selectedGroup);

  let name_label = name;
  let status_label = status;
  let style = {
    background: "#6c6b6b",
    opacity: 1,
    color: theme.surface,
  };

  switch (status) {
    case "N/D":
    case "N/R":
      style.opacity = 10;
      break;
    case "RUN":
      break;
    case "PASS":
      break;
    case "FAIL":
      break;
    case "SKIP":
      style.opacity = 0.4;
      break;
    case "WAIT":
      break;
    default:
      console.log("Unknown status: ", status);
      status_label = "--";
      style.opacity = 0.4;
      name_label = "UNK";
      break;
  }
  style.background = getStatusColor(status);

  // If the GFS TCP is not connected, grey out the group
  if (!isGfsTcpConnected) {
    style.background = "#6c6b6b";
    style.opacity = 0.4;
    status_label = "UNK";
  }

  return (
    <TestGroupStyle
      className="noselect"
      onClick={() => {
        setSelectedGroup(selectedGroup === name ? null : name);
      }}
      style={{
        border:
          selectedGroup === name ? "2px solid #ffa630" : "2px solid #6c6b6b",
      }}
    >
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

// The text box below the test group selection
const BitTestInfoBoxStyle = styled.div`
  width: 95%;
  height: auto;
  min-height: 2rem;
  background-color: black;
  padding: 2.5%;
`;
function BitTestInfoBox({ testGroupData, selectedGroup, bitInfo }) {
  const [infoText, setInfoText] = useState("test");
  const [testList, setTestList] = useState(null);
  const [inPreLaunch, setInPreLaunch] = useState(false);

  const { flightData, isGgsConnected, isGfsTcpConnected } =
    useContext(GwsGlobal);

  const getStatusText = (status) => {
    const StatusText = {
      "N/R": "not run",
      RUN: "running",
      PASS: "passed",
      FAIL: "failed",
      SKIP: "skipped",
      WAIT: "waiting",
    };

    if (status in StatusText) {
      return StatusText[status];
    } else {
      return "Unknown Status: " + status;
    }
  };

  const getTestGroupName = (group_id) => {
    if (group_id in testGroupData) {
      if ("description" in testGroupData[group_id]) {
        return testGroupData[group_id].description;
      }
      // console.log("Group ID: ", group_id, testGroupData[group_id]);
      return group_id;
    }
    return "Unknown";
  };

  const generateInfoText = (bitInfo) => {
    let text = `${getStatusText(bitInfo.bit_test_status)}`;
    // text += `Total Tests: ${bitInfo.total_tests}\n`;
    // text += `Passed Tests: ${bitInfo.passed_tests}\n`;
    // text += `Failed Tests: ${bitInfo.failed_tests}\n`;
    // text += `Skipped Tests: ${bitInfo.skipped_tests}\n`;
    // text += `Running Tests: ${bitInfo.running_tests}\n`;
    // text += `Seconds Since Last Test: ${bitInfo.seconds_since_last_test}\n`;

    return text;
  };

  useEffect(() => {
    // Display group specific test info when a group is selected
    if (selectedGroup === null) {
      setInfoText(generateInfoText(bitInfo));
      setTestList(null);
    } else if (selectedGroup in testGroupData) {
      setInfoText(generateInfoText(bitInfo));
      // setInfoText(`${selectedGroup} - ${testGroupData[selectedGroup].status}`);
      setTestList(testGroupData[selectedGroup].tests);
    } else {
      setInfoText("unknown");
      setTestList(null);
    }
    if ("flight_phase" in flightData) {
      if (flightData.flight_phase === "Pre-Launch") {
        setInPreLaunch(true);
      } else {
        setInPreLaunch(false);
      }
    }
  }, [flightData, testGroupData, selectedGroup, bitInfo, isGgsConnected]);

  return (
    <BitTestInfoBoxStyle>
      <div>
        {"BIT Status: "}
        <span
          style={{
            color: isGfsTcpConnected
              ? getStatusColor(bitInfo.bit_test_status)
              : "red",
          }}
        >
          {isGfsTcpConnected ? infoText : "Not Connected to GFS TCP"}
        </span>
      </div>
      {selectedGroup !== null && (
        <>
          <hr />
          <div>
            Test Group {getTestGroupName(selectedGroup)} -{" "}
            {testGroupData[selectedGroup].status}
          </div>
        </>
      )}
      {testList && (
        <div>
          {Object.keys(testList).map((key, index) => {
            return (
              <div
                key={index}
                style={{
                  padding: "5px",
                  display: "flex",
                  flexDirection: "row",
                  justifyContent: "space-between",
                }}
              >
                <span>{testList[key].name}</span>
                <span>{getStatusText(testList[key].status)}</span>
              </div>
            );
          })}
        </div>
      )}
    </BitTestInfoBoxStyle>
  );
}

export default function BitTestPanel() {
  const GroupDataSchema = {
    status: "N/R",
    tests: {}, // "0x0000":{name: "", status: ""}
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

  // Built in test info
  const [bitInfo, setBitInfo] = useState({
    bit_test_status: "N/D", // overall status of the BIT
    bit_test_ready: "n/d",
    total_tests: 0,
    passed_tests: 0,
    failed_tests: 0,
    skipped_tests: 0,
    running_tests: 0,
    seconds_since_last_test: 0,
    bit_test_running: 0,
    bit_test_start_time: 0,
    gfs_ready_to_perform_bit_test: 0,
    user_displayed_error: 0,
  });

  const { isGfsTcpConnected, ggsAddress } = useContext(GwsGlobal);

  const [selectedGroup, setSelectedGroup] = useState(null);
  const [runButtonText, setRunButtonText] = useState("false");
  const [runButtonDisabled, setRunButtonDisabled] = useState(false);

  const { data, isLoading, error, setNeedUpdate } = useApiGetData(
    "",
    "",
    "all",
    5000,
    "/api/flight_data/bit_test"
  );

  useEffect(() => {
    if (error) {
      console.log("Error: ", error);
      return;
    }

    if (isLoading) {
      // console.log("Loading...");
      return;
    }

    if (!data) {
      console.log("No data");
      return;
    }

    const bit_test_details = data.bit_test_details;
    setBitInfo({
      bit_test_ready: bit_test_details.bit_test_ready,
      bit_test_running: bit_test_details.bit_test_running,
      bit_test_start_time: bit_test_details.bit_test_start_time,
      bit_test_status: bit_test_details.bit_test_status,
      gfs_ready_to_perform_bit_test:
        bit_test_details.gfs_ready_to_perform_bit_test,
      user_displayed_error: bit_test_details.user_displayed_error,
    });

    const tests = data.tests;
    let new_test_data = {};
    for (let test_group in tests) {
      new_test_data[test_group] = {
        status: tests[test_group].status,
        tests: {},
      };

      for (let test in tests[test_group].tests) {
        new_test_data[test_group].tests[test] = {
          name: tests[test_group].tests[test].title,
          status: tests[test_group].tests[test].status,
        };
      }
    }

    setTestGroupData(new_test_data);
    // console.log("Test Data: ", tests);

    // console.log("New Test Data: ", new_test_data);
  }, [data, isLoading, error]);

  const requestStartTest = () => {
    // Send request to start the test

    fetch(`${ggsAddress}/api/flight_data/bit_test?action=start`, {
      method: "POST",
    })
      .then((response) => response.json())
      .then((data) => {
        console.log("Start Test Response: ", data);
        setNeedUpdate(true);
      });
  };

  const requestStopTest = () => {
    // Send request to stop the test
    console.log("Request to stop test");
  };

  const requestResetTest = () => {
    // Send request to reset the test
    console.log("Request to reset test - not implemented");
  };

  const bitTestButtonHandler = () => {
    if (bitInfo.bit_test_status === "N/R") {
      requestStartTest();
    } else if (bitInfo.bit_test_status === "RUN") {
      requestStopTest();
    } else {
      requestResetTest();
    }
  };

  // useEffect(() => {
  //   const interval = setInterval(() => {
  //     setTestGroupData({
  //       ...testGroupData,
  //     });
  //     setBitInfo({
  //       ...bitInfo,
  //     });
  //   }, 2000);
  //   return () => clearInterval(interval);
  // }, []);

  useEffect(() => {
    if (!isGfsTcpConnected) {
      setRunButtonDisabled(true);
      setRunButtonText("Not Connected");
      return;
    } else {
      setRunButtonDisabled(false);
    }

    if (bitInfo.bit_test_status === "N/R") {
      setRunButtonText("Run Test");
    } else if (bitInfo.bit_test_status === "RUN") {
      setRunButtonText("Stop Test");
    } else if (
      bitInfo.bit_test_status === "PASS" ||
      bitInfo.bit_status === "FAIL"
    ) {
      setRunButtonText("Reset Test");
    } else {
      setRunButtonText("");
    }
  }, [bitInfo, isGfsTcpConnected]);

  return (
    <div
      style={{
        padding: "10px",
        border: "2px solid #6c6b6b",
        borderRadius: "5px",
        marginBottom: "10px",
        marginTop: "10px",
      }}
    >
      <CardSectionTitle
        style={{
          justifyContent: "space-between",
        }}
      >
        <span>Pre-Launch BIT</span>
        <Tooltip text={"Run Built-In Test. Must be in the Pre-Launch phase."}>
          <StyButton
            onClick={bitTestButtonHandler}
            disabled={runButtonDisabled}
          >
            {runButtonText}
          </StyButton>
        </Tooltip>
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
              selectedGroup={selectedGroup}
              setSelectedGroup={setSelectedGroup}
            />
          );
        })}
      </div>
      <BitTestInfoBox
        testGroupData={testGroupData}
        selectedGroup={selectedGroup}
        bitInfo={bitInfo}
      />
    </div>
  );
}
