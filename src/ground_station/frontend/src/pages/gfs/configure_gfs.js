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
            <EditBox resource="gfs" category="general"/>
          </Card>
          <Card title="Debugging">
           <EditBox resource="gfs" category="debug"/>
          </Card>

          <Card title="Telemetry">
            <CardSectionTitle>General</CardSectionTitle>
            <EditBox resource="gfs" category="telemetry_general"/>
            <CardBreak />
            <CardSectionTitle>APRS</CardSectionTitle>
            <EditBox resource="gfs" category="telemetry_aprs" />
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
