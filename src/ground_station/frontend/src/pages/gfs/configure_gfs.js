import {
  PageTitle,
  PageContent,
  Card,
  CardBreak,
  CardSectionTitle,
  CardMasonryLayout,
} from '../../core/PageParts';
import { EditBox } from '../../components/Editable';

function ConfigureGfsPage() {
  return (
    <>
      <PageTitle>Giraffe Flight Software - Configure</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="General">
            <EditBox api_path="/gfs/general" />
          </Card>
          <Card title="Debugging">
            <EditBox api_path="/gfs/debugging" />
          </Card>

          <Card title="Telemetry">
            <CardSectionTitle>General</CardSectionTitle>
            <EditBox api_path="/gfs/telemetry/general" />
            <CardBreak />
            <CardSectionTitle>APRS</CardSectionTitle>
            <EditBox api_path="/gfs/telemetry/aprs" />
            <CardBreak />
            <CardSectionTitle>SSTV</CardSectionTitle>
            <EditBox api_path="/gfs/telemetry/sstv" />
            <CardBreak />
            <CardSectionTitle>Data Packets</CardSectionTitle>
            <EditBox api_path="/gfs/telemetry/data-packets" />
            {/* Radios */}
            {/* <EditBox api_path="/gfs/telemetry/radios" /> */}
          </Card>
          <Card title="Flight Procedures"></Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}
export default ConfigureGfsPage;
