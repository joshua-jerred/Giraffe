import styled from "styled-components";
import { useState, useRef, useContext, useEffect } from "react";
import Masonry, { ResponsiveMasonry } from "react-responsive-masonry";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faChevronUp } from "@fortawesome/free-solid-svg-icons";

import Tooltip from "../components/Tooltip";
import { GwsGlobal } from "../GlobalContext";

export const Page = styled.div`
  padding: 0 0px;
  color: ${(props) => props.theme.on_surface};
  min-width: 450px;
  max-width: ${(props) => props.theme.pages.max_width};
  margin: 0 ${(props) => props.theme.pages.side_margin};
  margin: 0 auto;
`;

const TitleExpandArrowStyle = styled.button`
  position: relative;
  font-size: 20px;
  left: -10px;

  background: rgba(0, 0, 0, 0);
  color: ${(props) => props.theme.primary};
  border: none;

  transition-duration: 0.5s;
  transform: rotate(${(props) => (props.isExpanded ? "180deg" : "0deg")});

  &:hover {
    -webkit-transform: rotate(180deg);
    cursor: pointer;
  }
`;

export const PageTitleStyle = styled.div`
  font-size: ${(props) => props.theme.fonts.page_title.size};
  font-family: ${(props) => props.theme.fonts.page_title.family};
  font-weight: ${(props) => props.theme.fonts.page_title.weight};
  font-style: ${(props) => props.theme.fonts.page_title.style};

  text-align: center;
  color: ;
  width: 100%;
  padding: 0px;
  margin: ${(props) => props.theme.pages.title_margin} 0;

  background: ${(props) => props.theme.surface};

  &:hover {
    cursor: pointer;
  }
`;

const PageTitleCollapsible = styled.div`
  overflow: hidden;
  transition: height 0.3s ease-out;
  border-top: 1px solid ${(props) => props.theme.primary_soft};
`;

const PageTitleContentStyle = styled.div`
  padding: 20px;
  font-family: ${(props) => props.theme.fonts.page_title_block_content.family};
  font-size: ${(props) => props.theme.fonts.page_title_block_content.size};
  font-weight: ${(props) => props.theme.fonts.page_title_block_content.weight};
  font-style: ${(props) => props.theme.fonts.page_title_block_content.style};
`;

export function PageTitle({ title, children }) {
  const [expanded, setExpanded] = useState(false);
  const [height, setHeight] = useState(0);
  const ref = useRef();

  const toggle = (e) => {
    e.preventDefault();
    setExpanded(!expanded);
    setHeight(ref.current.clientHeight);
  };

  const classes = `list-group-item ${expanded ? "is-expanded" : null}`;
  const currentHeight = expanded ? height : 0;

  return (
    <PageTitleStyle
      className={["noselect", classes].join(" ")}
      onClick={toggle}
    >
      <TitleExpandArrowStyle isExpanded={expanded}>
        <FontAwesomeIcon icon={faChevronUp} />
      </TitleExpandArrowStyle>
      {title}
      <PageTitleCollapsible style={{ height: `${currentHeight}px` }}>
        <PageTitleContentStyle ref={ref}>{children}</PageTitleContentStyle>
      </PageTitleCollapsible>
    </PageTitleStyle>
  );
}

export const PageImage = styled.img`
  position: absolute;
  top: 10px;
  right: -230px;
  width: 200px;
  z-index: -1;
`;

export const PageContentStyle = styled.div`
  position: relative;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  box-sizing: border-box;
  width: 100%;
  color: ${(props) => props.theme.on_surface};
  font-family: ${(props) => props.theme.fonts.primary};
  margin-bottom: 10px;
`;

export function PageContent({ children, image }) {
  return (
    <>
      <PageContentStyle>
        {children}
        {image && <PageImage src={image} />}
      </PageContentStyle>
    </>
  );
}

const CardStyle = styled.div`
  position: relative;
  background: ${(props) => props.theme.surface};
  color: ${(props) => props.theme.on_surface};
  margin-top: ${(props) => props.theme.cards.margin_v};
  width: fill-available;
  border-radius: ${(props) => props.theme.cards.border_radius};
`;

const CardTitle = styled.div`
  font-size: ${(props) => props.theme.fonts.card_title.size};
  font-family: ${(props) => props.theme.fonts.card_title.family};
  font-weight: ${(props) => props.theme.fonts.card_title.weight};
  font-style: ${(props) => props.theme.fonts.card_title.style};

  border-bottom: ${(props) => props.theme.cards.title_border_bottom};
  border-color: ${(props) => props.theme.primary_soft};
  padding-top: 0px;
  padding-left: 20px;
`;

const CardContent = styled.div`
  font-size: ${(props) => props.theme.fonts.card_body.size};
  font-family: ${(props) => props.theme.fonts.card_body.family};
  font-weight: ${(props) => props.theme.fonts.card_body.weight};
  font-style: ${(props) => props.theme.fonts.card_body.style};

  padding: ${(props) => props.theme.cards.padding};
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  box-sizing: border-box;
  width: 100%;
  padding: 2% 2%;
`;

export const CardRow = styled.div`
  display: grid;
  grid-template-columns: 50% 50%;
  box-sizing: border-box;
  width: 98%;
  gap: 2%;
`;

const NAV_COLLAPSED_BREAKPOINTS = { 500: 1, 900: 2, 1390: 3 };
const NAV_EXPANDED_BREAKPOINTS = { 650: 1, 1050: 2, 1640: 3 };

export function CardMasonryLayout({ children }) {
  const { navExpanded } = useContext(GwsGlobal);

  const [breakpoints, setBreakpoints] = useState(NAV_COLLAPSED_BREAKPOINTS);

  useEffect(() => {
    if (navExpanded) {
      setBreakpoints(NAV_EXPANDED_BREAKPOINTS);
    } else {
      setBreakpoints(NAV_COLLAPSED_BREAKPOINTS);
    }
  }, [navExpanded]);

  return (
    <ResponsiveMasonry columnsCountBreakPoints={breakpoints}>
      <Masonry gutter="10px">{children}</Masonry>
    </ResponsiveMasonry>
  );
}

export const Card = ({ title, children }) => {
  return (
    <CardStyle>
      {title ? <CardTitle>{title}</CardTitle> : null}
      <CardContent>{children}</CardContent>
    </CardStyle>
  );
};

export const CardBreak = styled.hr`
  border: ${(props) => props.theme.cards.card_break_border}
    ${(props) => props.theme.primary_soft};
  width: ${(props) => props.theme.cards.card_break_width};
  margin: ${(props) => props.theme.cards.card_break_margin};
`;

export const CardContentCentered = styled.div`
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
`;

export const CardSectionTitle = styled.div`
  font-size: ${(props) => props.theme.fonts.card_section_title.size};
  font-family: ${(props) => props.theme.fonts.card_section_title.family};
  width: 100%;
  display: flex;
  justify-content: center;
  align-items: center;
`;

// key: *value* w/ tooltip
const KeyDataPairWithTooltip = styled.ul`
  list-style-type: none;
  margin: 0;
  padding: 0;
  width: 100%;

  li {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
    margin-bottom: 0.5em;
    overflow: hidden;
    border-bottom: 1px solid #ccc;

    .sp1 {
      font-weight: bold;
      margin-right: 0.5em;
      font-size: 0.8em;
    }
  }
`;

// data: [
//    {Label: "label", value: "value", tooltip: "tooltip"},
export function LabelDataPair({ data, style }) {
  return (
    <KeyDataPairWithTooltip style={style}>
      {data.map((item) => (
        <Tooltip text={item.tooltip} specified_delay={200} key={item.key}>
          <li key={item.key}>
            <span className="sp1">{item.key} </span>
            <span>{item.value}</span>
          </li>
        </Tooltip>
      ))}
    </KeyDataPairWithTooltip>
  );
}
