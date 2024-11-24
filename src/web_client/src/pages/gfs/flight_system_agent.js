import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardBreak,
  CardSectionTitle,
} from "../../core/PageParts";

import { DataBlock } from "../../components/DataBlock";
import { EditBox } from "../../components/Editable";

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
          <Card title="Flight System Agent Status">
            <DataBlock resource="fsa" category="status" />
          </Card>
          <Card title="Flight System Agent Settings">
            <EditBox resource="fsa" category="settings" />
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
