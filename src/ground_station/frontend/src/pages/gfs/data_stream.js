import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from '../../core/PageParts';

function GfsDataStreamPage() {
  return (
    <>
      <PageTitle>Giraffe Ground Station - GFS Data Stream</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="card1"> </Card>
          <Card title="card2"> </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default GfsDataStreamPage;
