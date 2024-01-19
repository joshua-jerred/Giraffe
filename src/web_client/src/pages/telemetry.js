import { GGS_API } from "../api_interface/ggs_api";
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

import { StyButton } from "../components/styled/StyledComponents";

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

function MessageLog({ category }) {
  const { ggsAddress } = useContext(GwsGlobal);
  const { ggsConnectionStatus } = useContext(GGS_API);

  const [messages, setMessages] = useState([]);

  const encoded_category = encodeURIComponent(category);
  const path = `http://${ggsAddress}/api/gdl/telemetry?category=${encoded_category}`;

  const UPDATE_INTERVAL_MS = 1500;

  useEffect(() => {
    const loadData = async () => {
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
        })
        .catch((error) => {
          console.error(error);
        });
    };

    loadData(); // run once immediately
    const interval = setInterval(() => {
      loadData();
    }, UPDATE_INTERVAL_MS);
    return () => clearInterval(interval);
  }, [path, ggsConnectionStatus]);

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
  return <StyButton>hello</StyButton>;
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
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default TelemetryPage;
