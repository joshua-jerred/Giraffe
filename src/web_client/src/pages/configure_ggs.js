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
          <Card title="Ground Station Server Settings">
            <EditBox resource="ggs" category="ggs_server" />
          </Card>

          <Card title="Giraffe Flight Software Connection">
            <EditBox resource="ggs" category="gfs_connection" />
          </Card>

          <Card title="Ground Station Update Intervals">
            <EditBox resource="ggs" category="gfs_state_intervals" />
          </Card>

          <Card title="InfluxDB Setup">
            <EditBox resource="ggs" category="influx_db" />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default ConfigureGgsPage;
