// import { GGS_API } from "../api_interface/ggs_api";
import { GwsGlobal } from "../GlobalContext";

import { useState, useEffect, useContext } from "react";
import styled from "styled-components";

import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";
import { DataBlock } from "../components/DataBlock";
import Tooltip from "../components/Tooltip";

import { StyButton, StyInput } from "../components/styled/StyledComponents";

import { UseGenericGetApi } from "../api_interface/ggs_api";

const MessageLogListStyle = styled.ul`
  list-style-type: none;
  margin: 0;
  padding: 0;
  width: 100%;
`;

const MessageLogItemStyle = styled.li`
  display: flex;
  flex-direction: row;
  justify-content: space-between;
  margin-bottom: 0.5em;
`;

const MessageLogIdStyle = styled.span`
  font-size: 0.8em;
  font-weight: bold;
`;

function AprsFi() {
  const UPDATE_INTERVAL_MS = 5000;
  const { data, isLoading, error } = UseGenericGetApi(
    "/api/ggs/aprs_fi",
    UPDATE_INTERVAL_MS
  );

  if (isLoading) {
    return <p>Loading...</p>;
  }

  if (error) {
    return <p>Error: {error}</p>;
  }

  return (
    <div>
      <ul>
        {data &&
          data.map((item) => (
            <MessageLogItemStyle key={item.id}>
              {new Date(item.lasttime * 1000).toISOString()} - {item.altitude}m
              - {item.speed} km/h - {item.course} deg - {item.comment}
            </MessageLogItemStyle>
          ))}
      </ul>
    </div>
  );
}

function MessageLog({ category }) {
  const { ggsAddress, isGgsConnected } = useContext(GwsGlobal);

  const [messages, setMessages] = useState([]);
  const [error, setError] = useState(null);

  const encoded_category = encodeURIComponent(category);
  const path = `${ggsAddress}/api/gdl/telemetry?category=${encoded_category}`;

  const UPDATE_INTERVAL_MS = 1500;

  useEffect(() => {
    const loadData = async () => {
      if (!isGgsConnected) {
        setError("Not connected to GGS");
        return;
      }
      fetch(path)
        .then((response) => {
          if (!response.ok) {
            throw new Error("Failed to load data.");
          } else {
            return response.json();
          }
        })
        .then((data) => {
          setMessages(data);
          setError(null);
        })
        .catch((error) => {
          console.error(error);
          setError(error);
        });
    };

    loadData(); // run once immediately
    const interval = setInterval(() => {
      loadData();
    }, UPDATE_INTERVAL_MS);
    return () => clearInterval(interval);
  }, [path, isGgsConnected]);

  if (error) {
    return <p>Error: {error.message}</p>;
  }

  return (
    <MessageLogListStyle>
      {messages.map((message) => (
        <Tooltip
          text={message.timestamp}
          specified_delay={100}
          key={message.id}
        >
          <MessageLogItemStyle>
            <MessageLogIdStyle>
              {message.msg_id}:{message.type === "BROADCAST" ? "B" : "E"}
            </MessageLogIdStyle>
            <span>{message.message}</span>
          </MessageLogItemStyle>
        </Tooltip>
      ))}
    </MessageLogListStyle>
  );
}

function SendCommand() {
  const [command, setCommand] = useState("");

  return (
    <>
      <StyInput
        type="text"
        value={command}
        onChange={(e) => setCommand(e.target.value)}
      />
      <StyButton>send</StyButton>
    </>
  );
}

function TelemetryPage() {
  return (
    <>
      <PageTitle title="Telemetry">@todo - add a description here</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Data Link Stats">
            <DataBlock resource="gdl" category="status" />
          </Card>
          <Card title="Sent">
            <MessageLog category="sent_messages" />
          </Card>
          <Card title="Received">
            <MessageLog category="received_messages" />
          </Card>
          <Card title="Send Command">
            <SendCommand />
          </Card>
          <Card title="aprs.fi">
            <AprsFi />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default TelemetryPage;
