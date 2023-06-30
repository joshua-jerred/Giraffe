import { PageTitle, PageContent, Card } from "../core/PageParts";

import EditableTable from "../components/EditableTable";

function TestPage() {
  return (
    <>
      <PageTitle>Test Page</PageTitle>
      <PageContent>
        <Card title="Extensions">
          <EditableTable resource={"gfs"} category="extensions"></EditableTable>
        </Card>
      </PageContent>
    </>
  );
}

export default TestPage;
