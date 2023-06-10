import { Outlet } from 'react-router-dom';
import styled from 'styled-components';
import React from 'react';

import NavBar from './Nav';
import { Page } from './PageParts';
import StatusBar from './StatusBar';

import { useLocation } from 'react-router-dom';

import { GGS_WS } from '../api_interface/ws_api';

const Content = styled.div`
  margin-left: ${(props) =>
    props.navExpanded
      ? (props) => props.theme.nav.width.expanded
      : (props) => props.theme.nav.width.collapsed};
  background: ${(props) => props.theme.surface_container};
`;

function Layout() {
  const { sendPathMessage, ggsConnectionStatus } = React.useContext(GGS_WS);

  let direction = localStorage.getItem('nav_expanded') === 'true';
  const [navExpanded, setNavExpanded] = React.useState(direction);

  React.useEffect(() => {
    localStorage.setItem('nav_expanded', navExpanded);
  }, [navExpanded]);

  // Send the current path to the server
  const location = useLocation();
  React.useEffect(() => {
    if (ggsConnectionStatus === 'connected') {
      sendPathMessage(location.pathname);
    }
  }, [location, ggsConnectionStatus]);

  return (
    <>
      <NavBar navExpanded={navExpanded} setNavExpanded={setNavExpanded} />
      <Content navExpanded={navExpanded}>
        <Page>
          <StatusBar/>
          <Outlet />
        </Page>
      </Content>
    </>
  );
}

export default Layout;
