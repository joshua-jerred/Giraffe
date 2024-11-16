import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardContentCentered,
  CardBreak,
} from "../core/PageParts";
import gws_setup from "../assets/gws_setup.jpg";
import { useContext, useState } from "react";
import { GwsGlobal } from "../GlobalContext";
import { LocalItemEdit } from "../components/ItemEdit";
import { SwitchWithLabel } from "../components/styled/StyledComponents";
import { EditBox } from "../components/Editable";
import { useTheme } from "styled-components";

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
    unsafeMode,
    setUnSafeMode,
  } = useContext(GwsGlobal);

  const theme = useTheme();

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
        <CardMasonryLayout>
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
            <SwitchWithLabel
              checked={unsafeMode}
              setChecked={setUnSafeMode}
              label="Unsafe Mode"
              onColor={theme.error}
              offColor={theme.success}
            />
            {unsafeMode ? (
              <CardContentCentered
                style={{
                  backgroundColor: theme.error,
                  padding: "1rem",
                  borderRadius: "0.5rem",
                }}
                className="noselect"
              >
                <strong>WARNING</strong>
                <br />
                Unsafe mode is enabled. This allows the client to make changes
                to the Ground Station server during a flight. This mode should
                be disabled when not in use.
              </CardContentCentered>
            ) : null}
          </Card>
        </CardMasonryLayout>
      </PageContent>

      <PageTitle title="Ground Station Configuration">
        @todo - add a description here
      </PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Giraffe Flight Software Connection">
            <EditBox resource="ggs" category="gfs_connection" />
          </Card>

          <Card title="Giraffe Data Link Connection">
            <EditBox resource="ggs" category="gdl" />
          </Card>

          <Card title="Ground Station Settings">
            <EditBox resource="ggs" category="ggs_settings" />
          </Card>

          <Card title="Giraffe Data Link">
            <EditBox resource="gdl" category="all" />
          </Card>

          <Card title="InfluxDB Setup">
            <EditBox resource="ggs" category="influx_db" />
          </Card>

          <Card title="MySQL Setup">
            <EditBox resource="ggs" category="mysql" />
          </Card>

          <Card title="aprs.fi API">
            <EditBox resource="ggs" category="aprs_fi" />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default SetupPage;
