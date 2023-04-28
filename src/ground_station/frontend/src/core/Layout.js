import { Outlet } from 'react-router-dom';
import styled from 'styled-components';
import React from 'react';

import NavBar from './Nav';
import { Page } from './PageParts';
import StatusBar from './StatusBar';

import { useLocation } from 'react-router-dom';

import { GwsGlobal } from '../GlobalContext';
import { create } from 'giraffe-protocol/message';

const Content = styled.div`
  margin-left: ${(props) =>
    props.navExpanded
      ? (props) => props.theme.nav.width.expanded
      : (props) => props.theme.nav.width.collapsed};
  background: ${(props) => props.theme.surface_container};
`;

function Layout() {
  const { ggsConnectionStatus, sendJsonMessage } = React.useContext(GwsGlobal);

  let direction = localStorage.getItem('nav_expanded') === 'true';
  const [navExpanded, setNavExpanded] = React.useState(direction);

  React.useEffect(() => {
    localStorage.setItem('nav_expanded', navExpanded);
  }, [navExpanded]);

  const location = useLocation();
  React.useEffect(() => {
    if (ggsConnectionStatus === 'connected') {
      let path = location.pathname;
      let message = create('client', 'path', { path: path });
      sendJsonMessage(message);
    }
  }, [ggsConnectionStatus, location, sendJsonMessage]);

  return (
    <>
      <NavBar navExpanded={navExpanded} setNavExpanded={setNavExpanded} />
      <Content navExpanded={navExpanded}>
        <Page>
          <StatusBar
            status_content={[
              'This is the status bar',
              'OH SHIT, THE MUSTARD IS WATERY',
            ]}
          />
          <Outlet />
        </Page>
      </Content>
    </>
  );
}

export default Layout;
