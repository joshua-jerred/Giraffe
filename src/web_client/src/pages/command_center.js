import styled from "styled-components";
import React, { useEffect, useState, useContext } from "react";
import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardBreak,
  CardSectionTitle,
  CardContentCentered,
} from "../core/PageParts";
import {
  StyButton,
  StySelect,
  StyOption,
} from "../components/styled/StyledComponents";
import { DataBlock } from "../components/DataBlock";
import Tooltip from "../components/Tooltip";

import { Map } from "../components/map";

import { GwsGlobal } from "../GlobalContext";
import CommandsMetadataFile from "giraffe-protocol";
import { useApiGetData } from "../api_interface/ggs_api";

function MissionClock() {
  const [startTime, setStartTime] = useState(new Date());
  const [isClockValid, setIsClockValid] = useState(true);

  const [days, setDays] = useState(0);
  const [hours, setHours] = useState(0);
  const [minutes, setMinutes] = useState(0);
  const [seconds, setSeconds] = useState(0);

  const { data, isDataLoading, error } = useApiGetData(
    "flight_data",
    "mission_clock",
    "all",
    10000
  );

  useEffect(() => {
    if (!isDataLoading && !error && data) {
      try {
        let input_time = data.start_time;
        let new_start_time = new Date(input_time);
        if (isNaN(new_start_time)) {
          throw new Error("Invalid Date");
        }
        setStartTime(new_start_time);
        setIsClockValid(true);
      } catch (error) {
        console.error("Error parsing date", error);
        setIsClockValid(false);
      }
    }
  }, [data, isDataLoading, error]);

  useEffect(() => {
    if (!isClockValid) {
      return;
    }

    const interval = setInterval(() => {
      let ellapsed_time = new Date() - startTime;
      setDays(Math.floor(ellapsed_time / (1000 * 60 * 60 * 24)));
      setHours(Math.floor((ellapsed_time / (1000 * 60 * 60)) % 24));
      setMinutes(Math.floor((ellapsed_time / 1000 / 60) % 60));
      setSeconds(Math.floor((ellapsed_time / 1000) % 60));
    }, 1000);
    return () => clearInterval(interval);
  }, [startTime, isClockValid]);

  return (
    <div>
      <CardSectionTitle>Mission Clock</CardSectionTitle>
      {isDataLoading ? (
        <p>Loading...</p>
      ) : error ? (
        <p>Error: {error}</p>
      ) : (
        <div>
          {isClockValid ? (
            <div>
              <CardContentCentered>
                <h2>
                  {days}d {hours}h {minutes}m {seconds}s
                </h2>
                <p>Start Time: {startTime.toUTCString()}</p>
              </CardContentCentered>
            </div>
          ) : (
            <p>Invalid Start Time</p>
          )}
        </div>
      )}
    </div>
  );
}

const CommandsMetadata = CommandsMetadataFile.CommandMetadata;
const CommandSectionStyled = styled.ul`
  list-style-type: none;
  padding: 0;
  width: 100%;

  li {
    display: flex;
    flex-direction: row;
    flex-flow: row nowrap;
    justify-content: space-between;
    width: 100%;
    padding: 0.5em 0;

    > * {
      flex: auto;
      white-space: nowrap;
      // margin: 0px;
      // padding: 0em;
      // height: 100%;
      align-self: center;
    }

    > ${StySelect} {
      flex: 10%;
      margin-right: 1em;
    }

    > ${StyButton} {
      flex: 100%;
      max-width: 100px;
    }
  }
`;

const CommandItemTitle = styled.span`
  font-weight: bold;
  width: 50%;
  text-align: left;
  min-width: fit-content;
`;

function SingleCommand({ name, metadata, sendMethod, prefix }) {
  const { ggsAddress } = useContext(GwsGlobal);
  const [selectedOption, setSelectedOption] = useState(
    metadata.options ? metadata.options[0].value : ""
  );
  const [commandSentStatus, setCommandSentStatus] = useState("not_sent");

  const handleSendCommand = () => {
    const displayLastStatus = (status) => {
      setCommandSentStatus(status);
      setTimeout(() => {
        setCommandSentStatus("not_sent");
      }, 1000);
    };

    let command_string =
      "cmd/" + prefix + "/" + metadata.cmd_key + "/" + selectedOption;

    setCommandSentStatus("sending: " + command_string);
    fetch(ggsAddress + "/api/command", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({
        command_string: command_string,
        send_method: sendMethod,
      }),
    })
      .then((response) => response.json())
      .then((response) => {
        console.log("Command sent", response);
        displayLastStatus("command sent"); //response.message);
      })
      .catch((error) => {
        displayLastStatus("error");
        console.error("Error sending command", error);
      });
  };

  return (
    <li key={name}>
      <Tooltip text={metadata.details}>
        <CommandItemTitle>{metadata.name}</CommandItemTitle>
      </Tooltip>
      {metadata.options ? (
        <StySelect onChange={(e) => setSelectedOption(e.target.value)}>
          {metadata.options.map((option) => {
            return (
              <StyOption value={option.value} key={option.value}>
                {option.name}
              </StyOption>
            );
          })}
        </StySelect>
      ) : (
        <div></div>
      )}
      {commandSentStatus === "not_sent" ? (
        <StyButton onClick={handleSendCommand}>Send</StyButton>
      ) : (
        <p>{commandSentStatus}</p>
      )}
    </li>
  );
}

const SendMethodStyle = styled.div`
  display: flex;
  > * {
    margin: 0;
    flex: 1;
    align-self: center;
  }
  > span {
    text-align: right;
    flex: 2;
  }
`;

function CommandList() {
  const { isGfsTcpConnected, isGdlConnected, isUplinkConnected } =
    useContext(GwsGlobal);

  const ReadyColor = "#00ff00";
  const NoUplinkColor = "#ffcc00";
  const NotConnectedColor = "#ff0000";

  const [sendMethod, setSendMethod] = useState(
    localStorage.getItem("command_send_method") || "tcp_socket"
  );
  const [sendMethodStatus, setSendMethodStatus] = useState({
    status: "unknown",
    color: NotConnectedColor,
  });

  let Sections = [];
  for (let section in CommandsMetadata) {
    if (section === "FILE_META" || section === "general") continue;
    let Commands = [];
    let prefix = CommandsMetadata[section].SECTION_METADATA.prefix;
    for (let command in CommandsMetadata[section]) {
      if (command === "SECTION_METADATA") continue;
      Commands.push(
        <SingleCommand
          name={command}
          metadata={CommandsMetadata[section][command]}
          sendMethod={sendMethod}
          prefix={prefix}
          key={CommandsMetadata[section][command].value + command}
        />
      );
    }
    Sections.push(
      <div key={section}>
        <CardBreak />
        <CommandSectionStyled>
          <CardSectionTitle>{section}</CardSectionTitle>
          {Commands}
        </CommandSectionStyled>
      </div>
    );
  }

  React.useEffect(() => {
    const ValidSendMethods = ["tcp_socket", "gdl_broadcast", "gdl_exchange"];

    if (!ValidSendMethods.includes(sendMethod)) {
      setSendMethod("tcp_socket");
    }

    window.localStorage.setItem("command_send_method", sendMethod);
  }, [sendMethod]);

  React.useEffect(() => {
    if (sendMethod === "tcp_socket") {
      if (isGfsTcpConnected) {
        setSendMethodStatus({ status: "ready", color: ReadyColor });
      } else {
        setSendMethodStatus({
          status: "not connected",
          color: NotConnectedColor,
        });
      }
    } else if (sendMethod === "gdl_broadcast") {
      if (isGdlConnected) {
        setSendMethodStatus({ status: "ready", color: ReadyColor });
      } else {
        setSendMethodStatus({ status: "GDL down", color: NotConnectedColor });
      }
    } else if (sendMethod === "gdl_exchange") {
      if (isGdlConnected) {
        if (isUplinkConnected) {
          setSendMethodStatus({ status: "ready", color: ReadyColor });
        } else {
          setSendMethodStatus({
            status: "ready, no uplink",
            color: NoUplinkColor,
          });
        }
      } else {
        setSendMethodStatus({ status: "GDL down", color: NotConnectedColor });
      }
    } else {
      console.error("Unknown send method", sendMethod);
    }
    // console.log("Setting send method status");
    // setSendMethodStatus("checking..." + isGfsTcpConnected + isGdlConnected);
  }, [sendMethod, isGfsTcpConnected, isGdlConnected, isUplinkConnected]);

  return (
    <div>
      <CardSectionTitle>Send Method</CardSectionTitle>
      <SendMethodStyle>
        <StySelect
          onChange={(e) => setSendMethod(e.target.value)}
          value={sendMethod}
        >
          <StyOption value="tcp_socket">TCP Socket</StyOption>
          <StyOption value="gdl_broadcast">GDL Broadcast</StyOption>
          <StyOption value="gdl_exchange">GDL Exchange</StyOption>
        </StySelect>
        <span style={{ color: sendMethodStatus.color }}>
          {sendMethodStatus.status}
        </span>
      </SendMethodStyle>
      {Sections}
    </div>
  );
}

function CommandCenterPage() {
  return (
    <>
      <PageTitle title="Command Center">
        @todo - add a description here
      </PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Flight Status">
            <MissionClock />
            <CardBreak />
            <DataBlock resource="flight_data" category="general" />
          </Card>
          <Card title="Tracking">
            <Map />
            <CardBreak />
            <DataBlock resource="flight_data" category="location" />
          </Card>
          <Card title="Commands">
            <CommandList />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default CommandCenterPage;
