import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";
import { DataStreamBlock } from "../components/DataBlock";
import { Map } from "../components/map";

function CommandCenterPage() {
  return (
    <>
      <PageTitle>Giraffe Ground Station - Command Center</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Ground Station Status">
            <DataStreamBlock resource="ggs" category="status" />
          </Card>
          <Card title="Tracking">
            <Map />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default CommandCenterPage;
