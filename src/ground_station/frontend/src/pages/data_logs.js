import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from '../core/PageParts';
function DataLogsPage() {
  return (
    <>
      <PageTitle>Giraffe Ground Station - Data Logs</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="card1"> </Card>
          <Card title="card2"> </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default DataLogsPage;
