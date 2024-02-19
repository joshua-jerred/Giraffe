import styled from "styled-components";
import React, { useState, useContext } from "react";
import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardBreak,
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

const TemporaryCommandsMetadata =
  // Metadata for all GFS commands
  {
    FILE_META: {
      namespace: "cmd",
      enum_name: "CommandId",
      enum_type: "uint16_t",
      generate_string_map: "K_STRING_TO_COMMAND_MAP",
    },
    general: {
      unknown: {
        name: "Unknown",
        details:
          "Placeholder, used as a default/fallback value. This commands will be responded to with and error.",
        value: "0x0000",
      },
    },
    flight_runner: {
      SECTION_METADATA: {
        prefix: "flr",
      },
      shutdown_system: {
        name: "Shutdown System",
        details: "Tells the flight runner to shutdown.",
        value: "0x1000",
        cmd_key: "sdn",
      },
      start_module: {
        name: "Start Module",
        details:
          "Tells the flight runner to start a module given a 3 letter module identifier.",
        value: "0x1001",
        cmd_key: "mst",
        options: [
          // {
          //   "name": "Flight Runner",
          //   "value": "flr"
          // },
          {
            name: "Data Module",
            value: "dat",
          },
          {
            name: "Telemetry Module",
            value: "tel",
          },
        ],
      },
      stop_module: {
        name: "Stop Module",
        details:
          "Tells the flight runner to stop a module given a 3 letter module identifier.",
        value: "0x1002",
        cmd_key: "msp",
      },
      restart_module: {
        name: "Restart Module",
        details:
          "Tells the flight runner to restart a module given a 3 letter module identifier.",
        value: "0x1003",
        cmd_key: "mrt",
      },
    },
    data_module: {
      SECTION_METADATA: {
        prefix: "dat",
      },
      clear_all_errors: {
        name: "Clear All Errors",
        details: "Clears all errors from the error frame",
        value: "0x2000",
        cmd_key: "cae",
      },
      clear_specific_error: {
        name: "Clear a Specific Error",
        details:
          "Clears a specific error/diagnostic code from the error frame given an error code (uint16_t) in hex.",
        value: "0x2001",
        cmd_key: "cse",
      },
    },
  };

const CommandSectionStyled = styled.ul`
  list-style-type: none;
  padding: 0;
  width: 100%;

  li {
    display: flex;
    flex-direction: row;
    width: 100%;
    padding: 0.5em 0;

    > * {
      margin: 0;
      flex: 1;
    }
  }
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
        displayLastStatus(response.message);
      })
      .catch((error) => {
        displayLastStatus("error");
        console.error("Error sending command", error);
      });
  };

  return (
    <li key={name}>
      <Tooltip text={metadata.details}>{metadata.name}</Tooltip>
      {metadata.options && (
        <StySelect onChange={(e) => setSelectedOption(e.target.value)}>
          {metadata.options.map((option) => {
            return <StyOption value={option.value}>{option.name}</StyOption>;
          })}
        </StySelect>
      )}
      {commandSentStatus === "not_sent" ? (
        <StyButton onClick={handleSendCommand}>Send</StyButton>
      ) : (
        <p>{commandSentStatus}</p>
      )}
    </li>
  );
}

function CommandList() {
  const [sendMethod, setSendMethod] = useState("tcp_socket");

  let CommandsMetadata = TemporaryCommandsMetadata;

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
          key={CommandsMetadata[section][command].value}
        />
      );
    }
    Sections.push(
      <>
        <CommandSectionStyled key={section}>
          <h2>{section}</h2>
          {Commands}
        </CommandSectionStyled>
        <CardBreak />
      </>
    );
  }

  return (
    <div>
      <h2>Send Method</h2>
      <StySelect onChange={(e) => setSendMethod(e.target.value)}>
        <StyOption value="tcp_socket">TCP Socket</StyOption>
        <StyOption value="gdl_broadcast">GDL Broadcast</StyOption>
        <StyOption value="gdl_exchange">GDL Exchange</StyOption>
      </StySelect>
      <CardBreak />
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
