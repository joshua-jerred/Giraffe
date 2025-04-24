import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardBreak,
  CardSectionTitle,
} from "../../core/PageParts";

import { StyButton } from "../../components/styled/StyledComponents";

import { DataBlock } from "../../components/DataBlock";
import { EditBox } from "../../components/Editable";

import styled from "styled-components";

import { ApiRequestButton } from "../../components/api_request_button";

export const ButtonBox = styled.div`
  display: flex;
  margin: 1em;
  & > * {
    margin: 0.5em;
    padding: 0.3em 1em;
  }
`;

function DaemonControl() {
  return (
    <ButtonBox
      style={{
        display: "flex",
        justifyContent: "space-around",
        alignItems: "center",
      }}
    >
      <ApiRequestButton
        api_endpoint="/api/fsa/control?action=start"
        title="Start"
        request_data_callback={() => {
          return { action: "start" };
        }}
      />
      <ApiRequestButton
        api_endpoint="/api/fsa/control?action=stop"
        title="Stop"
        request_data_callback={() => {
          return { action: "stop" };
        }}
      />
    </ButtonBox>
  );
}

function FlightSystemAgentPage() {
  return (
    <>
      <PageTitle title="Flight System Agent">
        The Flight System Agent (FSA) is the main agent/daemon running on the
        flight system. It is responsible for keeping the flight software up and
        running. It also facilitates updating the flight software.
      </PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Flight System Agent Settings">
            <EditBox resource="fsa" category="settings" />
          </Card>
          <Card title="Control">
            <DaemonControl />
          </Card>
          <Card title="Flight System Agent Status">
            <DataBlock resource="fsa" category="status" />
          </Card>
          {/* <CardBreak /> */}
          {/* <Card title="Calculated Data"> */}
          {/* <DataBlock resource="fsa" category="status" /> */}
          {/* </Card> */}

          {/* <CardBreak /> */}
          {/* <CardSectionTitle>IMU</CardSectionTitle> */}
          {/* <DataStreamBlock
              resource="gfs"
              category="position_imu"
              stream_name="position_imu"
            /> */}
          {/* </Card> */}
          {/* <Card title="Telemetry Data"></Card>

          <Card title="Extensions">
            <DataStreamBlock resource="gfs" category="extension_module_stats" />
          </Card>

          <Card title="Environmental">
            <DataStreamBlock resource="gfs" category="environmental" />
          </Card> */}
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default FlightSystemAgentPage;
