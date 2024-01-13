import { Outlet } from "react-router-dom";
import styled from "styled-components";
import React from "react";

import NavBar from "./Nav";
import { Page } from "./PageParts";
import StatusBar from "./StatusBar";

import { useLocation } from "react-router-dom";

import { GGS_API } from "../api_interface/ws_api";

const Content = styled.div`
  margin-left: ${(props) =>
    props.navExpanded
      ? (props) => props.theme.nav.width.expanded
      : (props) => props.theme.nav.width.collapsed};
  background: ${(props) => props.theme.surface_container};
`;

function Layout() {
  const { removeAllStreams } = React.useContext(GGS_API);

  let direction = localStorage.getItem("nav_expanded") === "true";
  const [navExpanded, setNavExpanded] = React.useState(direction);

  React.useEffect(() => {
    localStorage.setItem("nav_expanded", navExpanded);
  }, [navExpanded]);

  // Remove all streams when navigating away from a page (this is a bad solution)
  // const location = useLocation();
  // React.useEffect(() => {
  //   removeAllStreams();
  // }, [location]);

  return (
    <>
      <NavBar navExpanded={navExpanded} setNavExpanded={setNavExpanded} />
      <Content navExpanded={navExpanded}>
        <Page>
          <StatusBar />
          <Outlet />
        </Page>
      </Content>
    </>
  );
}

export default Layout;
