import { NavLink } from "react-router-dom";
import styled from "styled-components";
import React from "react";

const NavIconStyled = styled.i`
  font-size: 2em;
  position: relative;
  display: table-cell;
  width: 50px;
  height: 36px;
  text-align: center;
  vertical-align: middle;
  color: ${(props) => props.theme.on_surface};
`;
const NavLabelStyled = styled.span`
  font-family: ${(props) => props.theme.fonts.nav_bar_link_label.family};
  font-size: ${(props) => props.theme.fonts.nav_bar_link_label.size};
  font-weight: ${(props) => props.theme.fonts.nav_bar_link_label.weight};
  font-style: ${(props) => props.theme.fonts.nav_bar_link_label.style};

  color: ${(props) => props.theme.on_surface};

  position: relative;
  display: table-cell;
  width: 50px;
  height: 36px;
  text-align: center;
  vertical-align: middle;
`;

const GenericNavLine = styled.li`
  display: table;
  display: table;
  width: 100%;
`;
const GenericNavLineContent = styled.div`
  display: table-cell;
  vertical-align: middle;
  font-family: "Roboto Mono", monospace;
`;

/* -------- Nav Item Components -------- */
const NavSubItems = styled.ul`
  padding: 0;
  margin: 0;
`;

const NavItemLinkStyled = styled(NavLink)`
  position: relative;
  display: table;
  border-collapse: collapse;
  border-spacing: 0;
  color: ${(props) => props.theme.on_surface};
  font-size: 14px;
  text-decoration: none;
  -webkit-transform: translateZ(0) scale(1, 1);
  -webkit-transition: all 0.1s linear;
  transition: all 0.1s linear;
  &:hover {
    background: ${(props) => props.theme.surface_hover};
    color: ;
  }

  &.active > * {
    color: ${(props) => props.theme.primary};
  }
  &.active.parent > * {
    color: ${(props) => props.theme.primary_soft};
  }

  & + ${NavSubItems} {
    display: none;
  }
  &.active + ${NavSubItems} {
    display: block;
  }
`;
const NavItemTitleStyled = styled.span`
  font-family: ${(props) => props.theme.fonts.nav_bar_link_title.family};
  font-size: ${(props) => props.theme.fonts.nav_bar_link_title.size};
  font-weight: ${(props) => props.theme.fonts.nav_bar_link_title.weight};
  font-style: ${(props) => props.theme.fonts.nav_bar_link_title.style};

  position: relative;
  display: table-cell;
  vertical-align: middle;
  width: calc(${(props) => props.theme.nav.width.expanded} - 50px);
`;

const NavItemStyled = styled.li`
  position: relative;
  display: block;
  width: ${(props) => props.theme.nav.width.expanded};
  outline: 0;
  margin: 0;
  padding: 0;

  &.parent {
    border-bottom: 1px solid ${(props) => props.theme.surface_hover_hard};
    border-top: 1px solid ${(props) => props.theme.surface_hover_hard};
    margin-top: -1px;
    margin-bottom: -1px;
  }
  &.parent:hover {
    background: ${(props) => props.theme.surface_hover_soft};
  }
  &.parent:hover > ${NavSubItems} {
    display: block;
  }
`;

const NavItem = ({ title, icon, path, label, children }) => {
  let display_icon = icon ? true : false;
  let isParent = children ? true : false;
  return (
    <NavItemStyled className={isParent ? "parent" : null}>
      <NavItemLinkStyled to={path} className={isParent ? "parent" : null}>
        {display_icon ? (
          <NavIconStyled className={`fa ${icon}`}></NavIconStyled>
        ) : (
          <NavLabelStyled>{label}</NavLabelStyled>
        )}
        <NavItemTitleStyled className="nav-text">{title}</NavItemTitleStyled>
      </NavItemLinkStyled>
      {isParent ? <NavSubItems>{children}</NavSubItems> : null}
    </NavItemStyled>
  );
};

const NavFooter = styled.ul`
  position: absolute;
  bottom: 0;
  display: block;
  width: ${(props) => props.theme.nav.width.expanded};
  outline: 0;
  margin: 0;
  padding: 0;
  margin-bottom: 50px;
  color: ${(props) => props.theme.on_surface};
`;

/* -------- Nav Bar -------- */
const NavPin = styled.button`
  display: none;
  position: absolute;
  top: 10px;
  right: 0px;
  width: 25px;
  height: 25px;
  background: ${(props) => props.theme.surface};
  color: ${(props) => props.theme.primary};
  border: none;
  cursor: pointer;
`;

const NavBarStyled = styled.nav`
  background: ${(props) => props.theme.surface};
  position: fixed;
  top: 0;
  bottom: 0;
  height: 100%;
  width: ${(props) =>
    props.isExpanded
      ? (props) => props.theme.nav.width.expanded
      : (props) => props.theme.nav.width.collapsed};
  left: 0;
  padding-top: ${(props) => props.theme.nav.width.collapsed};
  overflow: ${(props) => (props.isExpanded ? "visible" : "hidden")};
  -webkit-transition: ${(props) => props.theme.transitions.nav_bar};
  transition: ${(props) => props.theme.transitions.nav_bar};
  -webkit-transform: translateZ(0) scale(1, 1);
  -webkit-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  -o-user-select: none;
  user-select: none;
  z-index: 100;

  &:hover > ${NavPin} {
    display: block;
  }
  & > * {
    -webkit-transition: ${(props) => props.theme.transitions.nav_bar};
    transition: ${(props) => props.theme.transitions.nav_bar};
  }
`;

function NavBar({ navExpanded, setNavExpanded }) {
  return (
    <>
      <NavBarStyled isExpanded={navExpanded}>
        <NavPin onClick={() => setNavExpanded(!navExpanded)}>
          <i className={`fa fa-chevron-${navExpanded ? "left" : "right"}`}></i>
        </NavPin>

        <NavItem title="Command Center" icon="fa-terminal" path="/" />

        <NavItem title="Telemetry" label="TLM" path={"/telemetry"} />
        <NavItem title="Data Logs" label="DAT" path={"/data-logs"} />
        <NavItem title="Configure GGS" label="CFG" path="/configure" />

        <NavItem title="GFS" icon="fa-rocket" path={"/gfs"}>
          <NavItem title="Data Monitor" label="DAT" path="/gfs/data-monitor" />
          <NavItem title="Configure" label="CFG" path="/gfs/configure" />
        </NavItem>

        <NavFooter>
          <NavItem
            title="Documentation"
            icon="fa-book"
            path="/docs"
            border-top="true"
          />
          <NavItem
            title="Setup"
            icon="fa-gear"
            path="/setup"
            border-top="true"
          />
          <GenericNavLine>
            <GenericNavLineContent></GenericNavLineContent>
          </GenericNavLine>
        </NavFooter>
      </NavBarStyled>
    </>
  );
}
export default NavBar;
