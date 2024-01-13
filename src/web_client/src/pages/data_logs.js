import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";
import { DataStreamBlock } from "../components/DataBlock";

function DataLogsPage() {
  return (
    <>
      <PageTitle>Giraffe Ground Station - Data Logs</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="card1">
            <DataStreamBlock
              resource="ggs"
              category="status"
              stream_name="status"
            />
          </Card>
          <Card title="card2"> </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default DataLogsPage;
