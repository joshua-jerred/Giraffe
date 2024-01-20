import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";

function AprsFi() {
  return <p>aprs.fi data</p>;
}

function TrackingPage() {
  return (
    <>
      <PageTitle title={"Tracking"}>@todo - add a description here</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="card1"> </Card>
          <Card title="aprs.fi">
            <AprsFi />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default TrackingPage;
