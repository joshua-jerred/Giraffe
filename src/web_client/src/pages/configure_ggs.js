import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";
import { EditBox } from "../components/Editable";

function ConfigureGgsPage() {
  return (
    <>
      <PageTitle title="Ground Station Configuration">
        @todo - add a description here
      </PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Giraffe Flight Software Connection">
            <EditBox resource="ggs" category="gfs_connection" />
          </Card>

          <Card title="Ground Station Settings">
            <EditBox resource="ggs" category="ggs_settings" />
          </Card>

          <Card title="Giraffe Data Link">
            <EditBox resource="ggs" category="gdl" />
          </Card>

          <Card title="InfluxDB Setup">
            <EditBox resource="ggs" category="influx_db" />
          </Card>

          <Card title="Ground Station Server Settings">
            <EditBox resource="ggs" category="aprs_fi" />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default ConfigureGgsPage;
