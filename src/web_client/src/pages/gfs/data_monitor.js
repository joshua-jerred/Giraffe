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
} from "../../components/DynamicDataBlock";

function GfsDataMonitorPage() {
  return (
    <>
      <PageTitle>Giraffe Ground Station - GFS Data Stream</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Module Statuses">
            <DataStreamBlock
              resource="gfs"
              category="modules_statuses"
              stream_name="modules_statuses"
            />
          </Card>
          <Card title="Server Module Stats">
            <DataStreamBlock
              resource="gfs"
              category="server_module_stats"
              stream_name="server_module_stats"
            />
          </Card>
          <Card title="System Info">
            <DataStreamBlock
              resource="gfs"
              category="system_info"
              stream_name="system_info"
            />
          </Card>
          <Card title="Position Data">
            <CardSectionTitle>GPS</CardSectionTitle>
            {/* <DataStreamBlock
              resource="gfs"
              category="position_gps"
              stream_name="position_gps"
            /> */}
            <CardBreak />
            <CardSectionTitle>IMU</CardSectionTitle>
            {/* <DataStreamBlock
              resource="gfs"
              category="position_imu"
              stream_name="position_imu"
            /> */}
          </Card>
          <Card title="Telemetry Data"></Card>

          <Card title="Extensions"></Card>

          <Card title="Modules"></Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default GfsDataMonitorPage;
