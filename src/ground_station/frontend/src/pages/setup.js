import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardContentCentered,
  CardBreak,
} from "../core/PageParts";
import gws_setup from "../assets/gws_setup.jpg";
import { useContext, useState } from "react";
import { GwsGlobal } from "../GlobalContext";
import { LocalItemEdit } from "../components/ItemEdit";
import { SwitchWithLabel } from "../components/styled/StyledComponents";
import { EditBox } from "../components/Editable";
import { useTheme } from "styled-components";
import styled from "styled-components";
import React, { useEffect } from "react";
import { CopyToClipboard } from "react-copy-to-clipboard";

import {
  ApiRequestButton,
  ApiRequestDropdown,
} from "../components/api_request_button";
import { DataBlock } from "../components/DataBlock";
import { UseGenericGetApi } from "../api_interface/ggs_api";
import Tooltip from "../components/Tooltip";

import { unixTimeToLocal } from "../core/clock_strings";
import { StyButton, SelectMenu } from "../components/styled/StyledComponents";

const GgsLogStyle = styled.ul`
  list-style-type: none;
  margin: 0;
  padding: 0;
  width: 100%;
  height: 20rem;
  overflow-y: auto;
  font-family: ${(props) => props.theme.fonts.small_paragraph.family};
  font-size: ${(props) => props.theme.fonts.small_paragraph.size};
  font-weight: ${(props) => props.theme.fonts.small_paragraph.weight};
  font-style: ${(props) => props.theme.fonts.small_paragraph.style};
`;

const LogItemStyle = styled.li`
  margin-top: 0.1rem;
  padding: 0.1em;
  transition: background-color 0.3s;
  border-radius: 0.3em;

  :nth-child(even) {
    background-color: ${(props) => props.theme.surface_alt_2};
  }
  :nth-child(odd) {
    background-color: ${(props) => props.theme.surface_alt};
  }

  :hover {
    background-color: ${(props) => props.theme.surface_hover_hard};
    cursor: pointer;

    > .overflow > ul {
      background-color: ${(props) => props.theme.surface};
      cursor: default;
    }
  }

  .mainContent {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
    padding-bottom: 1.2em;
    overflow: hidden;
    align-items: center;
    vertical-align: middle;

    .sp1 {
      font-weight: bold;
      margin-right: 0.5em;
    }
  }

  .overflow {
    color: ${(props) => props.theme.on_surface};
    display: ${(props) => (props.open ? "block" : "none")};
    padding: 0.5em;

    ul {
      list-style-type: none;
      padding: 0.5em;
      margin: 0;
      border: 1px solid #ccc;
      border-radius: 0.3em;
      background-color: ${(props) => props.theme.surface_alt};

      li {
        display: flex;
        justify-content: space-between;

        padding: 0.5em;
        margin: 0;
        border-bottom: 1px solid #ccc;
      }
    }
  }
`;

function LogItem({ level, timestamp, message, database_id }) {
  const [open, setOpen] = React.useState(false);

  const local_time = unixTimeToLocal(0 / 1000);
  // const timestamp_long = isoMilliseconds(timestamp);

  const copy_content = `[${database_id} ${level}] ${timestamp} (${local_time}) - ${message}`;

  return (
    <LogItemStyle open={open}>
      <div
        className="mainContent"
        onClick={() => {
          setOpen(!open);
        }}
      >
        <span className="sp1">{level}</span>
        <span>{message}</span>
      </div>
      <div className="overflow">
        <ul>
          <li>
            <span>Level: {level}</span>
            <span>ID: {database_id}</span>
          </li>
          <li
            style={{
              display: "flex",
              justifyContent: "space-between",
            }}
          >
            <span>UTC: {timestamp}</span>
            <span>Local: {local_time}</span>
          </li>
          <li>{message}</li>
          <li>
            <CopyToClipboard text={copy_content}>
              <StyButton onClick={() => {}}>copy to clipboard</StyButton>
            </CopyToClipboard>
            <ApiRequestButton
              api_endpoint={"/api/ggs/log"}
              title={"delete"}
              request_data_callback={() => {
                return { id: database_id };
              }}
              success_callback={() => {
                console.log("success");
              }}
              api_method={"DELETE"}
            />
          </li>
        </ul>
      </div>
    </LogItemStyle>
  );
}

function GgsLog() {
  const UPDATE_INTERVAL_MS = 5000;
  const { data, isLoading, error } = UseGenericGetApi(
    "/api/ggs/log",
    UPDATE_INTERVAL_MS
  );
  const [logIsEmpty, setLogIsEmpty] = React.useState(false);

  React.useEffect(() => {
    if (data && typeof data === "object" && data.hasOwnProperty("length")) {
      if (data.length === 0) {
        setLogIsEmpty(true);
      } else {
        setLogIsEmpty(false);
      }
    }
  }, [data]);

  if (error) {
    return <p>Error: {error}</p>;
  }

  return (
    <div>
      {logIsEmpty ? (
        <p
          style={{
            textAlign: "center",
          }}
        >
          there are no log items.
        </p>
      ) : null}
      <GgsLogStyle>
        {data &&
          data.map((item) => (
            <LogItem
              level={item.level}
              timestamp={item.createdAt}
              message={item.message}
              key={item.id}
              database_id={item.id}
            />
          ))}
      </GgsLogStyle>
      <ApiRequestDropdown
        api_endpoint={"/api/ggs/log/level"}
        data_key={"level"}
        options={["error", "warning", "info", "debug"]}
        title={"set log level"}
      />
      <div
        style={{
          display: "flex",
          justifyContent: "center",
          marginTop: "1rem",
        }}
      ></div>
      {isLoading === true ? "loading" : "loaded"}
    </div>
  );
}

function GroundStationDatabaseUtilities() {
  return (
    <div
      style={{
        display: "flex",
        flexDirection: "column",
        justifyContent: "center",
        marginTop: "1rem",
        alignItems: "center",
        gap: "1rem",
      }}
    >
      <ApiRequestButton
        api_endpoint={"/api/ggs/log"}
        title={"delete all ground station log entries"}
        request_data_callback={() => {
          return { id: "all" };
        }}
        api_method={"DELETE"}
        confirmation_message={
          "Are you sure you want to delete all log entries? This action will permanently delete all GGS/Ground Station log entries."
        }
        unsafe={true}
      />

      <ApiRequestButton
        api_endpoint={"/api/gdl/aprs_telemetry"}
        title={"delete all aprs telemetry data"}
        request_data_callback={() => {
          return { id: "all" };
        }}
        api_method={"DELETE"}
        confirmation_message={
          "Are you sure you want to delete all log entries? This action will permanently delete all APRS telemetry data."
        }
        unsafe={true}
      />

      <ApiRequestDropdown
        api_endpoint={"/api/gdl/aprs_telemetry/fake"}
        title={"add fake aprs telemetry data packet"}
        options={["1", "10", "100", "500", "1000"]}
        // api_method={"POST"}
        unsafe={true}
        no_loading={true}
        data_key={"num"}
      />
    </div>
  );
}

function SetupPage() {
  // For GWS Client Configuration
  const [TestConnection, setTestConnection] = useState("not tested");
  const {
    clientDarkTheme,
    setClientDarkTheme,
    clientName,
    ggsAddress,
    setClientName,
    setGgsAddress,
    unsafeMode,
    setUnSafeMode,
  } = useContext(GwsGlobal);

  const theme = useTheme();

  // Test client connection to the GGS server
  function testGgs(address) {
    if (!address.includes("http")) {
      setTestConnection("Address must include http:// or https://");
      return;
    }
    setTestConnection("testing...");
    fetch(address + "/api/status")
      .then((response) => response.json())
      .then((data) => {
        if (data.status === "ok") {
          setTestConnection("connection ok");
        } else {
          setTestConnection("error: " + data);
        }
      })
      .catch((error) => {
        setTestConnection("error: " + error);
      });

    setTimeout(() => {
      setTestConnection("not tested");
    }, 2000);
  }

  function testClientName(name) {
    const re = /^[a-zA-Z_]*$/;
    if (
      !re.test(name) ||
      name.length < 3 ||
      name.length > 15 ||
      name === "not_set"
    ) {
      setTestConnection("Err: 3-15 chars, a-z, A-Z, _");
      return;
    }
    setTestConnection("client name ok");
  }

  return (
    <>
      <PageTitle title="Client Setup">
        This is where you can configure your web client to connect to the Ground
        Station server. Once connected, you can configure the Ground Station.
      </PageTitle>
      <PageContent image={gws_setup}>
        <CardMasonryLayout>
          <Card title="Client Configuration">
            <LocalItemEdit
              title="Giraffe Ground Station Server Address"
              state={ggsAddress}
              setState={setGgsAddress}
              tooltip="The address of the Ground Station server. For example: http://localhost:8321"
              onTest={(value) => {
                testGgs(value);
              }}
            />
            <LocalItemEdit
              title="Client Name/Identifier"
              state={clientName}
              setState={setClientName}
              tooltip="Enter a name or identifier for this client. This will be used to identify this client in the Ground Station server."
              onTest={(value) => {
                testClientName(value);
              }}
            />
            {TestConnection !== "not tested" ? (
              <CardContentCentered>{TestConnection}</CardContentCentered>
            ) : null}
            <SwitchWithLabel
              checked={clientDarkTheme}
              setChecked={setClientDarkTheme}
              label="Dark Theme"
            />
            <SwitchWithLabel
              checked={unsafeMode}
              setChecked={setUnSafeMode}
              label="Unsafe Mode"
              onColor={theme.error}
              offColor={theme.success}
            />
            {unsafeMode ? (
              <CardContentCentered
                style={{
                  backgroundColor: theme.error,
                  padding: "1rem",
                  borderRadius: "0.5rem",
                }}
                className="noselect"
              >
                <strong>WARNING</strong>
                <br />
                Unsafe mode is enabled. This allows the client to make changes
                to the Ground Station server during a flight. This mode should
                be disabled when not in use.
              </CardContentCentered>
            ) : null}
          </Card>
        </CardMasonryLayout>
      </PageContent>

      <PageTitle title="Ground Station Data">
        @todo - add a description here
      </PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Ground Station Status">
            <DataBlock resource="ggs" category="status" stream_name="status" />
          </Card>

          <Card title="Ground Station Log">
            <GgsLog />
          </Card>

          <Card title="Ground Station Database Utilities">
            <GroundStationDatabaseUtilities />
          </Card>
        </CardMasonryLayout>
      </PageContent>

      <PageTitle title="Ground Station Configuration">
        @todo - add a description here
      </PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Giraffe Flight Software Connection">
            <EditBox resource="ggs" category="gfs_connection" />
          </Card>

          <Card title="Giraffe Data Link Connection">
            <EditBox resource="ggs" category="gdl" />
          </Card>

          <Card title="Ground Station Settings">
            <EditBox resource="ggs" category="ggs_settings" />
          </Card>

          <Card title="Giraffe Data Link">
            <EditBox resource="gdl" category="all" />
          </Card>

          <Card title="InfluxDB Setup">
            <EditBox resource="ggs" category="influx_db" />
          </Card>

          <Card title="MySQL Setup">
            <EditBox resource="ggs" category="mysql" />
          </Card>

          <Card title="aprs.fi API">
            <EditBox resource="ggs" category="aprs_fi" />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default SetupPage;
