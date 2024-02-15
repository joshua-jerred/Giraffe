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
      <PageTitle title="Command Center">
        @todo - add a description here
      </PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Flight Status">
            <DataBlock resource="flight_data" category="general" />
          </Card>
          <Card title="Tracking">
            <Map />
          </Card>
          <Card title="Location">
            <DataBlock resource="flight_data" category="location" />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default CommandCenterPage;
