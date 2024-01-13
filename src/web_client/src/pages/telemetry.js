import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";
import { DataBlock } from "../components/DataBlock";

function TelemetryPage() {
  return (
    <>
      <PageTitle>Giraffe Ground Station - Telemetry</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Data Link Stats">
            <DataBlock resource="gdl" category="status" />
          </Card>
          <Card title="card2"> </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default TelemetryPage;
