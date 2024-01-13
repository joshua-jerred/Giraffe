import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardBreak,
  CardSectionTitle,
} from "../../core/PageParts";

import {
  DataStreamBlock,
  NestedDataStreamBlock,
} from "../../components/DataBlock";

function GfsDataMonitorPage() {
  return (
    <>
      <PageTitle>Giraffe Flight System Data Monitor</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Calculated Data">
            <DataStreamBlock resource="gfs" category="calculated_data" />
          </Card>
          <Card title="Module Statuses">
            <DataStreamBlock resource="gfs" category="modules_statuses" />
          </Card>
          <Card title="Server Module Stats">
            <DataStreamBlock resource="gfs" category="server_module_stats" />
          </Card>
          <Card title="System Info">
            <DataStreamBlock resource="gfs" category="system_info" />
          </Card>
          <Card title="Data Log">
            <DataStreamBlock resource="gfs" category="data_log_stats" />
          </Card>
          <Card title="Data Stream">
            <DataStreamBlock resource="gfs" category="stream_stats" />
          </Card>
          <Card title="Location">
            <DataStreamBlock resource="gfs" category="location_data" />
          </Card>
          <Card title="Stream Stats">
            <DataStreamBlock resource="gfs" category="stream_stats" />
          </Card>
          <Card title="Environmental">
            <DataStreamBlock resource="gfs" category="environmental" />
          </Card>
          <Card title="Extension Module">
            <DataStreamBlock resource="gfs" category="extension_module_stats" />
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
