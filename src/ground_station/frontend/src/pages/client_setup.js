import {
  PageTitle,
  PageContent,
  Card,
  CardRow,
  CardContentCentered,
} from '../core/PageParts';
import gws_setup from '../assets/gws_setup.jpg';
import { useContext, useState } from 'react';
import { GwsGlobal } from '../GlobalContext';
import { LocalItemEdit } from '../components/ItemEdit';
import { Switch } from '../components/StyledComponents';

function ClientSetupPage() {
  // For GWS Client Configuration
  const [TestConnection, setTestConnection] = useState('not tested');
  const { ggsSocketAddress, setGgsSocketAddress, darkMode, setDarkMode } =
    useContext(GwsGlobal);

  // Test client connection to the GWS server
  function TestGws(address) {
    setTestConnection('testing...');
    fetch(address + '/gws/status')
      .then((response) => response.json())
      .then((data) => {
        if (data.status === 'ok') {
          setTestConnection('ok');
        } else {
          setTestConnection('error: ' + data);
        }
      })
      .catch((error) => {
        setTestConnection('error: ' + error);
      });

    setTimeout(() => {
      setTestConnection('not tested');
    }, 2000);
  }

  return (
    <>
      <PageTitle>GWS Setup</PageTitle>
      <PageContent image={gws_setup}>
        <CardRow>
          <Card title="Client Configuration">
            <label>Dark Theme</label>
            <Switch checked={darkMode} setChecked={setDarkMode} />
            <LocalItemEdit
              title="GWS Server Address"
              state={ggsSocketAddress}
              setState={setGgsSocketAddress}
              tooltip="The address of the GWS server. For example: http://localhost:8321"
              onTest={(value) => {
                TestGws(value);
              }}
            />
            {TestConnection !== 'not tested' ? (
              <CardContentCentered>{TestConnection}</CardContentCentered>
            ) : null}
          </Card>
        </CardRow>
      </PageContent>
    </>
  );
}

export default ClientSetupPage;
