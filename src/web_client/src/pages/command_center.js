import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";
import { DataBox } from "../components/StaticDataBlock";

function CommandCenterPage() {
  return (
    <>
      <PageTitle>Giraffe Ground Station - Command Center</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Ground Station Status">
          <DataBox resource="ggs" category="status" />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default CommandCenterPage;
