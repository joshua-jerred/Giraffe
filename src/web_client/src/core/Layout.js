import { Outlet } from "react-router-dom";
import styled from "styled-components";
import React from "react";

import NavBar from "./Nav";
import { Page } from "./PageParts";
import StatusBar from "./StatusBar";

const Content = styled.div`
  margin-left: ${(props) =>
    props.navExpanded
      ? (props) => props.theme.nav.width.expanded
      : (props) => props.theme.nav.width.collapsed};
  background: ${(props) => props.theme.surface_container};
  padding: 0px ${(props) => props.theme.pages.side_margin};
`;

function Layout() {
  let direction = localStorage.getItem("nav_expanded") === "true";
  const [navExpanded, setNavExpanded] = React.useState(direction);

  React.useEffect(() => {
    localStorage.setItem("nav_expanded", navExpanded);
  }, [navExpanded]);

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
