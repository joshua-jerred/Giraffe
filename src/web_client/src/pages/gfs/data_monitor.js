import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardBreak,
  CardSectionTitle,
} from "../../core/PageParts";

import { DataBlock, NestedDataStreamBlock } from "../../components/DataBlock";

function GfsDataMonitorPage() {
  return (
    <>
      <PageTitle>Giraffe Flight System Data Monitor</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Calculated Data">
            <DataBlock resource="gfs" category="calculated_data" />
          </Card>
          <Card title="Module Statuses">
            <DataBlock resource="gfs" category="modules_statuses" />
          </Card>
          <Card title="Server Module Stats">
            <DataBlock resource="gfs" category="server_module_stats" />
          </Card>
          <Card title="System Info">
            <DataBlock resource="gfs" category="system_info" />
          </Card>
          <Card title="Data Log">
            <DataBlock resource="gfs" category="data_log_stats" />
          </Card>
          <Card title="Data Stream">
            <DataBlock resource="gfs" category="stream_stats" />
          </Card>
          <Card title="Location">
            <DataBlock resource="gfs" category="location_data" />
          </Card>
          <Card title="Stream Stats">
            <DataBlock resource="gfs" category="stream_stats" />
          </Card>
          <Card title="Environmental">
            <DataBlock resource="gfs" category="environmental" />
          </Card>
          <Card title="Extension Module">
            <DataBlock resource="gfs" category="extension_module_stats" />
          </Card>
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

export default GfsDataMonitorPage;
