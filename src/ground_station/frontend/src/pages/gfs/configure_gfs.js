import {
  PageTitle,
  PageContent,
  Card,
  CardBreak,
  CardSectionTitle,
  CardMasonryLayout,
} from "../../core/PageParts";
import { EditBox } from "../../components/Editable";

function ConfigureGfsPage() {
  return (
    <>
      <PageTitle>Giraffe Flight Software - Configure</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="General">
            <EditBox resource="gfs" category="general" />
          </Card>
          <Card title="Debugging">
            <EditBox resource="gfs" category="debug" />
          </Card>

          <Card title="Flight Procedures">
            {/* <EditBox resource="gfs" category="flight_procedures" /> */}
          </Card>

          <Card title="Extensions">
            {/* <EditBox resource="gfs" category="extensions" /> */}
          </Card>

          <Card title="Data Loging">
            {/* <EditBox resource="gfs" category="data_log" /> */}
          </Card>

          <Card title="Radios">
            {/* <EditBox resource="gfs" category="radios" /> */}
          </Card>
          
          <Card title="Telemetry">
            <CardSectionTitle>General</CardSectionTitle>
            <EditBox resource="gfs" category="telemetry_general" />
            <CardBreak />
            <CardSectionTitle>APRS</CardSectionTitle>
            <EditBox resource="gfs" category="telemetry_aprs" />
            <CardBreak />
            <CardSectionTitle>SSTV</CardSectionTitle>
            <EditBox resource="gfs" category="telemetry_sstv" />
            <CardBreak />
            <CardSectionTitle>Data Packets</CardSectionTitle>
            <EditBox resource="gfs" category="telemetry_data_packets" />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}
export default ConfigureGfsPage;
