import { Outlet } from "react-router-dom";
import styled from "styled-components";
import React from "react";

import NavBar from "./Nav";
import { Page } from "./PageParts";
import StatusBar from "./StatusBar";

import { GwsGlobal } from "../GlobalContext";

const Content = styled.div`
  margin-left: ${(props) =>
    props.navExpanded
      ? (props) => props.theme.nav.width.expanded
      : (props) => props.theme.nav.width.collapsed};
  background: ${(props) => props.theme.surface_container};
  padding: 0px ${(props) => props.theme.pages.side_margin};
`;

function Layout() {
  const { navExpanded, setNavExpanded } = React.useContext(GwsGlobal);

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
