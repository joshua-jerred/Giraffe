import styled from "styled-components";
import React from "react";
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

function LogItem({ level, timestamp, message }) {
  const [open, setOpen] = React.useState(false);

  const local_time = unixTimeToLocal(timestamp / 1000);
  const timestamp_long = isoMilliseconds(timestamp);

  const copy_content = `${timestamp_long} (${local_time}) [${level}] ${message}`;

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
            {timestamp_long}
            <CopyToClipboard text={copy_content}>
              <StyButton
                style={{ margin: "0.0em", zIndex: "100" }}
                onClick={() => {
                  console.log("copy");
                }}
              >
                copy
              </StyButton>
            </CopyToClipboard>
          </li>
          <li>{local_time}</li>
          <li>{level}</li>
          <li>{message}</li>
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

  if (error) {
    return <p>Error: {error}</p>;
  }

  return (
    <div>
      <GgsLogStyle>
        {data &&
          data.map((item) => (
            <LogItem
              level={item.level}
              timestamp={item.timestamp}
              message={item.message}
              key={item.timestamp}
            />
          ))}
      </GgsLogStyle>
      <ApiRequestDropdown
        api_endpoint={"/api/ggs/log/level"}
        data_key={"level"}
        options={["error", "warning", "info", "debug"]}
        title={"Set Log Level"}
      />
      {isLoading === true ? "loading" : "loaded"}
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
      <PageTitle title="Ground Station Configuration">
        @todo - add a description here
      </PageTitle>
    </>
  );
}

export default DataLogsPage;
