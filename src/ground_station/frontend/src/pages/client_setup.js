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
import { SwitchWithLabel } from '../components/StyledComponents';

function ClientSetupPage() {
  // For GWS Client Configuration
  const [TestConnection, setTestConnection] = useState('not tested');
  const {
    clientDarkTheme,
    setClientDarkTheme,
    clientName,
    ggsAddress,
    setClientName,
    setGgsAddress,
  } = useContext(GwsGlobal);

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
      <PageTitle>Ground Station Client Setup</PageTitle>
      <PageContent image={gws_setup}>
        <CardRow>
          <Card title="Client Configuration">
            <LocalItemEdit
              title="Giraffe Ground Station Server Address"
              state={ggsAddress}
              setState={setGgsAddress}
              tooltip="The address of the Ground Station server. For example: http://localhost:8321"
              onTest={(value) => {
                TestGws(value);
              }}
            />
            <LocalItemEdit
              title="Client Name/Identifier"
              state={clientName}
              setState={setClientName}
              tooltip="Enter a name or identifier for this client. This will be used to identify this client in the Ground Station server."
            />
            {TestConnection !== 'not tested' ? (
              <CardContentCentered>{TestConnection}</CardContentCentered>
            ) : null}
            <SwitchWithLabel
              checked={clientDarkTheme}
              setChecked={setClientDarkTheme}
              label="Dark Theme"
            />
          </Card>
        </CardRow>
      </PageContent>
    </>
  );
}

export default ClientSetupPage;
