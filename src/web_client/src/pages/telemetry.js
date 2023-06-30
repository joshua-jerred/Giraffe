import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from '../core/PageParts';

function TelemetryPage() {
  return (
    <>
      <PageTitle>Giraffe Ground Station - Telemetry</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="card1"> </Card>
          <Card title="card2"> </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default TelemetryPage;
