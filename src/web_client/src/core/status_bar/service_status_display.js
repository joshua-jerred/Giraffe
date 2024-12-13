import React, { useContext } from "react";
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

function ServiceStatusDisplay() {
  const { serviceStatuses, isGgsConnected, flightData } = useContext(GwsGlobal);

  return (
    <ServiceGrid>
      <StatusChip abbreviation="GGS" status="CONNECTED" />
      <StatusChip abbreviation="GDL" status="DISCONNECTED" />
      <StatusChip abbreviation="GFS" status="DISABLED" />
      <StatusChip abbreviation="FSA" status="UNKNOWN" />
    </ServiceGrid>
  );
}

export default ServiceStatusDisplay;
