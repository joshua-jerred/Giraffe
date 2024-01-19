import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardBreak,
  CardSectionTitle,
} from "../../core/PageParts";

import { DataBlock } from "../../components/DataBlock";

function GfsDataMonitorPage() {
  return (
    <>
      <PageTitle title="GFS Data Monitor">
        The GFS Data Monitor page is used to debug the Giraffe Flight Computer
        while it's still on the ground. It displays various data points from
        inside of the flight software. This data is sent to the ground station
        (backend) via a TCP socket. This means that this page is only useful
        prior to a flight.
        <br />
        <br />
        Note: The server module must be enabled within the flight software for
        the ground station to be able to connect.
      </PageTitle>
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
          <Card title="Telemetry Module">
            <DataBlock resource="gfs" category="telemetry_module_stats" />
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
