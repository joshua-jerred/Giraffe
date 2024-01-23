import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";

function TrackingPage() {
  return (
    <>
      <PageTitle title={"Tracking"}>@todo - add a description here</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="card1"> </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default TrackingPage;
