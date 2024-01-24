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
      <PageTitle title="GFS Configuration">
        @todo - add a description here
      </PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="General">
            <EditBox resource="gfs" category="general" />
          </Card>
          <Card title="Data Module - General/Data File Log">
            <EditBox resource="gfs" category="data_module_data" />
          </Card>

          <Card title="Data Module - Logging">
            <EditBox resource="gfs" category="data_module_log" />
          </Card>

          <Card title="Console Module">
            <EditBox resource="gfs" category="console_module" />
          </Card>

          <Card title="Data Logging">
            <EditBox resource="gfs" category="data_module_log" />
          </Card>

          <Card title="Server Module">
            <EditBox resource="gfs" category="server_module" />
          </Card>

          <Card title="System Module">
            <EditBox resource="gfs" category="system_module" />
          </Card>

          <Card title="Telemetry">
            <CardSectionTitle>General</CardSectionTitle>
            <EditBox resource="gfs" category="telemetry" />
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

          <Card title="Extensions">
            {/* <EditBox resource="gfs" category="system_module" /> */}
          </Card>

          <Card title="Flight Procedures">
            {/* <EditBox resource="gfs" category="system_module" /> */}
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}
export default ConfigureGfsPage;
