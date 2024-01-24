import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../../core/PageParts";
import { GfsEditBox } from "../../components/Editable";

function ConfigureGfsPage() {
  return (
    <>
      <PageTitle title="GFS Configuration">
        @todo - add a description here
      </PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="General">
            <GfsEditBox resource="gfs" category="general" />
          </Card>
          <Card title="Data Module - General/Data File Log">
            <GfsEditBox resource="gfs" category="data_module_data" />
          </Card>

          <Card title="Data Module - Logging">
            <GfsEditBox resource="gfs" category="data_module_log" />
          </Card>

          <Card title="Console Module">
            <GfsEditBox resource="gfs" category="console_module" />
          </Card>

          <Card title="Data Logging">
            <GfsEditBox resource="gfs" category="data_module_log" />
          </Card>

          <Card title="Server Module">
            <GfsEditBox resource="gfs" category="server_module" />
          </Card>

          <Card title="System Module">
            <GfsEditBox resource="gfs" category="system_module" />
          </Card>

          <Card title="Telemetry - General">
            <GfsEditBox resource="gfs" category="telemetry" />
          </Card>

          <Card title="Telemetry - APRS">
            <GfsEditBox resource="gfs" category="telemetry_aprs" />
          </Card>

          <Card title="Telemetry - SSTV">
            <GfsEditBox resource="gfs" category="telemetry_sstv" />
          </Card>

          <Card title="Telemetry - Data Packets">
            <GfsEditBox resource="gfs" category="telemetry_data_packets" />
          </Card>

          <Card title="Extensions">
            {/* <EditBox resource="gfs" category="system_module" /> */}
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}
export default ConfigureGfsPage;
