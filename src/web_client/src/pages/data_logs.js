import styled from "styled-components";
import React, { useEffect } from "react";
import { CopyToClipboard } from "react-copy-to-clipboard";

import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardContentCentered,
} from "../core/PageParts";
import { DataBlock } from "../components/DataBlock";
import { UseGenericGetApi } from "../api_interface/ggs_api";
import Tooltip from "../components/Tooltip";
import {
  ApiRequestButton,
  ApiRequestDropdown,
} from "../components/api_request_button";

import {
  unixTimeToLongUtc,
  unixTimeToShortUtc,
  isoMilliseconds,
  unixTimeToLocal,
} from "../core/clock_strings";
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

  const local_time = unixTimeToLocal(timestamp / 1000);
  const timestamp_long = isoMilliseconds(timestamp);

  const copy_content = `[${database_id} ${level}] ${timestamp_long} (${local_time}) - ${message}`;

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
            <span>UTC: {timestamp_long}</span>
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
              timestamp={item.timestamp}
              message={item.message}
              key={item.timestamp}
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

function DataLogsPage() {
  return (
    <>
      <PageTitle title={"Data"}>@todo - add a description here</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Ground Station Status">
            <DataBlock resource="ggs" category="status" stream_name="status" />
          </Card>

          <Card title="Ground Station Log">
            <GgsLog />
          </Card>
        </CardMasonryLayout>
      </PageContent>
      <PageTitle title="Database">@todo - add a description here</PageTitle>
      <CardMasonryLayout>
        <Card title="Ground Station Database Utilities">
          <GroundStationDatabaseUtilities />
        </Card>
      </CardMasonryLayout>
    </>
  );
}

export default DataLogsPage;
