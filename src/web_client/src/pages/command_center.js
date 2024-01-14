import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";
import { DataBlock } from "../components/DataBlock";
import { Map } from "../components/map";

function CommandCenterPage() {
  return (
    <>
      <PageTitle>Giraffe Ground Station - Command Center</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Flight Status">
            <DataBlock resource="flight_data" category="general" />
          </Card>
          <Card title="Tracking">
            <Map />
          </Card>
          <Card title="Location">
            {/* <DataBlock resource={"gdl"} category={"location"} /> */}
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default CommandCenterPage;
