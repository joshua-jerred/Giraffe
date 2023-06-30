import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from '../core/PageParts';

function TrackingPage() {
  return (
    <>
      <PageTitle>Giraffe Ground Station - Tracking</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="card1"> </Card>
          <Card title="card2"> </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default TrackingPage;
