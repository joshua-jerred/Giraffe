import React from "react";
import styled from "styled-components";

const TooltipBox = styled.div`
  color: ${(props) => props.theme.tooltip_text};
  background-color: ${(props) => props.theme.tooltip_background};

  min-width: 250px;
  max-width: 600px;
  overflow: hidden;
  width: fit-content;
  border-radius: ${(props) => props.theme.components.tooltip.border_radius};
  padding: ${(props) => props.theme.components.tooltip.padding};
  transition: ${(props) => props.theme.transitions.default};

  visibility: hidden;
  position: absolute;
  text-align: center;
  z-index: 101;
  bottom: ${(props) => props.vertical_position};
  opacity: 0;
`;

const TooltipContainer = styled.span`
  position: relative;
  &:hover ${TooltipBox} {
    transition-delay: ${(props) =>
      props.specified_delay ||
      props.theme.components.tooltip.transition_delay}ms;
    visibility: visible;
    opacity: 1;
  }
`;

const Tooltip = ({
  text,
  children,
  vertical_position = "125%",
  specified_delay = null,
  style = {},
}) => {
  return (
    <TooltipContainer specified_delay={specified_delay} style={style}>
      <TooltipBox vertical_position={vertical_position}>{text}</TooltipBox>
      {children}
    </TooltipContainer>
  );
};

export default Tooltip;
