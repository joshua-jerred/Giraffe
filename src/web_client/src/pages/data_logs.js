import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";
import { DataBlock } from "../components/DataBlock";

function DataLogsPage() {
  return (
    <>
      <PageTitle title={"Data Logs"}>@todo - add a description here</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="card1">
            <DataBlock resource="ggs" category="status" stream_name="status" />
          </Card>
          <Card title="card2"> </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default DataLogsPage;
