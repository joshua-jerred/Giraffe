import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";
import { DataBox } from "../components/StaticDataBlock";

import { GwsGlobal } from "../GlobalContext";

function CommandCenterPage() {
  return (
    <>
      <PageTitle>Giraffe Ground Station</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="this is a card title">
          <DataBox resource="ggs" category="status" />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default CommandCenterPage;
