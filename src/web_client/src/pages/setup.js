import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardContentCentered,
} from "../core/PageParts";
import gws_setup from "../assets/gws_setup.jpg";
import { useContext, useState } from "react";
import { GwsGlobal } from "../GlobalContext";
import { LocalItemEdit } from "../components/ItemEdit";
import { SwitchWithLabel } from "../components/styled/StyledComponents";
import { EditBox } from "../components/Editable";

function SetupPage() {
  // For GWS Client Configuration
  const [TestConnection, setTestConnection] = useState("not tested");
  const {
    clientDarkTheme,
    setClientDarkTheme,
    clientName,
    ggsAddress,
    setClientName,
    setGgsAddress,
  } = useContext(GwsGlobal);

  // Test client connection to the GGS server
  function testGgs(address) {
    if (!address.includes("http")) {
      setTestConnection("Address must include http:// or https://");
      return;
    }
    setTestConnection("testing...");
    fetch(address + "/api/status")
      .then((response) => response.json())
      .then((data) => {
        if (data.status === "ok") {
          setTestConnection("connection ok");
        } else {
          setTestConnection("error: " + data);
        }
      })
      .catch((error) => {
        setTestConnection("error: " + error);
      });

    setTimeout(() => {
      setTestConnection("not tested");
    }, 2000);
  }

  function testClientName(name) {
    const re = /^[a-zA-Z_]*$/;
    if (
      !re.test(name) ||
      name.length < 3 ||
      name.length > 15 ||
      name === "not_set"
    ) {
      setTestConnection("Err: 3-15 chars, a-z, A-Z, _");
      return;
    }
    setTestConnection("client name ok");
  }

  return (
    <>
      <PageTitle title="Client Setup">
        This is where you can configure your web client to connect to the Ground
        Station server. Once connected, you can configure the Ground Station.
      </PageTitle>
      <PageContent image={gws_setup}>
        <Card title="Client Configuration">
          <LocalItemEdit
            title="Giraffe Ground Station Server Address"
            state={ggsAddress}
            setState={setGgsAddress}
            tooltip="The address of the Ground Station server. For example: http://localhost:8321"
            onTest={(value) => {
              testGgs(value);
            }}
          />
          <LocalItemEdit
            title="Client Name/Identifier"
            state={clientName}
            setState={setClientName}
            tooltip="Enter a name or identifier for this client. This will be used to identify this client in the Ground Station server."
            onTest={(value) => {
              testClientName(value);
            }}
          />
          {TestConnection !== "not tested" ? (
            <CardContentCentered>{TestConnection}</CardContentCentered>
          ) : null}
          <SwitchWithLabel
            checked={clientDarkTheme}
            setChecked={setClientDarkTheme}
            label="Dark Theme"
          />
        </Card>
      </PageContent>

      <PageTitle title="Ground Station Configuration">
        @todo - add a description here
      </PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Giraffe Flight Software Connection">
            <EditBox resource="ggs" category="gfs_connection" />
          </Card>

          <Card title="Ground Station Settings">
            <EditBox resource="ggs" category="ggs_settings" />
          </Card>

          <Card title="Giraffe Data Link">
            <EditBox resource="ggs" category="gdl" />
          </Card>

          <Card title="InfluxDB Setup">
            <EditBox resource="ggs" category="influx_db" />
          </Card>

          <Card title="Ground Station Server Settings">
            <EditBox resource="ggs" category="aprs_fi" />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default SetupPage;
