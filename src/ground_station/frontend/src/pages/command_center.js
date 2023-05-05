import { useState, useEffect, useContext } from "react";
import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";
import {
  Button,
  Input,
  Select,
  Option,
  WarningButton,
  NeutralButton,
} from "../components/StyledComponents";

import { GwsGlobal } from "../GlobalContext";

function CommandCenterPage() {
  const { sendJsonMessage, lastMessage } = useContext(GwsGlobal);
  const [input, setInput] = useState("");
  const [messages, setMessages] = useState([]);

  useEffect(() => {
    if (lastMessage !== undefined && lastMessage !== null) {
      setMessages((messages) => [...messages, lastMessage]);
    }
  }, [lastMessage]);

  useEffect(() => {
  }, [input]);

  const onsend = () => {
    let msg = {
      source: "client",
      type: "get",
      body: { resource: "ggs", category: "setup" },
    };
    console.log(msg, "sent");
    sendJsonMessage(msg);
  };
  return (
    <>
      <PageTitle>Giraffe Ground Station</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="this is a card title">
            <Input onChange={(e) => setInput(e.target.value)} />
            <Button onClick={(e) => onsend()}>Send</Button>
          </Card>
          <Card title="card 1">This is the first card in a card row.</Card>
          <Card title="card 2">This is the second card in a card row.</Card>
          <Card title="themed components">
            <WarningButton>button</WarningButton>
            <NeutralButton>button</NeutralButton>
            <p></p>
            <Select>
              <Option>opt1</Option>
              <Option>opt2</Option>
            </Select>
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default CommandCenterPage;
