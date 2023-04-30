import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from '../core/PageParts';
import { EditBox } from '../components/Editable';
import { DataBox } from '../components/DataBlock';

function ConfigureGgsPage() {
  return (
    <>
      <PageTitle>Giraffe Ground Station - Configuration</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Ground Station Settings">
            <EditBox resource="ggs" category="gfs_connection" />
          </Card>
          <Card title="Ground Stations System Information">
            <DataBox api="ggs" category="data" />
          </Card>
          <Card title="card2"> </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default ConfigureGgsPage;
