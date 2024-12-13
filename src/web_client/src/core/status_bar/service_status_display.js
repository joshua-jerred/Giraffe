import React, { useContext, useEffect } from "react";
import styled from "styled-components";
import Masonry from "react-responsive-masonry";

import { GwsGlobal } from "../../GlobalContext";
import Tooltip from "../../components/Tooltip";
import StatusChip from "./status_chip";

const ServiceGrid = styled.div`
  // border: 1px solid red;

  display: grid;
  grid-template-columns: repeat(3, 1fr);
  grid-gap: 0.5em;

  margin: auto;

  @media (max-width: 768px) {
    grid-template-columns: repeat(2, 1fr);
  }
`;

const MAIN_SERVICES = {
  ggs: "DISCONNECTED",
  gdl: "UNKNOWN",
  gfs: "UNKNOWN",
  fsa: "UNKNOWN",
};

function ServiceStatusDisplay() {
  const { serviceStatuses, isGgsConnected, flightData } = useContext(GwsGlobal);

  const [mainServices, setMainServices] = React.useState(MAIN_SERVICES);

  useEffect(() => {
    // If GGS is not connected, set all services to default.
    if (!isGgsConnected) {
      setMainServices(MAIN_SERVICES);
      return;
    }

    let newServices = { ...mainServices };
    newServices.ggs = "CONNECTED";

    if (serviceStatuses.hasOwnProperty("gdl")) {
      newServices.gdl = serviceStatuses.gdl;
    }

    if (serviceStatuses.hasOwnProperty("gfs")) {
      newServices.gfs = serviceStatuses.gfs;
    }

    if (serviceStatuses.hasOwnProperty("fsa")) {
      newServices.fsa = serviceStatuses.fsa;
    }

    // console.log("asdsaa", newServices);
    console.log(serviceStatuses);
    setMainServices(newServices);
  }, [isGgsConnected, serviceStatuses, flightData]);

  return (
    <ServiceGrid>
      {Object.keys(mainServices).map((service) => {
        return (
          <StatusChip
            key={service}
            abbreviation={service}
            status={mainServices[service]}
          />
        );
      })}
    </ServiceGrid>
  );
}

export default ServiceStatusDisplay;
