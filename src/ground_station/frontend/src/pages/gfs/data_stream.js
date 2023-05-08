import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardBreak,
  CardSectionTitle,
} from "../../core/PageParts";

import { DataStreamBlock, NestedDataStreamBlock } from "../../components/DynamicDataBlock";

function GfsDataStreamPage() {
  return (
    <>
      <PageTitle>Giraffe Ground Station - GFS Data Stream</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Status">
            <NestedDataStreamBlock resource="gfs" category="gfs_status" stream_name="gfs_status" />
          </Card>
          <Card title="Critical Data">
            <DataStreamBlock resource="gfs" category="critical" stream_name="critical" />
          </Card>
          <Card title="Position Data">
            <CardSectionTitle>GPS</CardSectionTitle>
            <DataStreamBlock resource="gfs" category="position_gps" stream_name="position_gps" />
            <CardBreak />
            <CardSectionTitle>IMU</CardSectionTitle>
            <DataStreamBlock resource="gfs" category="position_imu" stream_name="position_imu" />
          </Card>
          <Card title="Environmental Data">
            
          </Card>
          <Card title="Telemetry Data">
          </Card>


          <Card title="Extensions">
          </Card>

          <Card title="Modules">
          </Card>

        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default GfsDataStreamPage;
