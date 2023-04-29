import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from '../core/PageParts';

function TrackingPage() {
  return (
    <>
      <PageTitle>Test Page</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="card1"> </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default TrackingPage;
