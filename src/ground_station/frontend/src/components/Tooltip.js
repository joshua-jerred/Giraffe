import React from "react";
import styled from "styled-components";

const TooltipBox = styled.div`
  color: ${(props) => props.theme.tooltip_text};
  background-color: ${(props) => props.theme.tooltip_background};
  font-family: ${(props) => props.theme.fonts.mono};
  font-size: ${(props) => props.theme.fonts.sizes.small};

  min-width: 150px;
  max-width: 600px;
  // overflow: hidden;
  // width: fit-content;
  border-radius: ${(props) => props.theme.components.tooltip.border_radius};
  padding: ${(props) => props.theme.components.tooltip.padding};
  transition: ${(props) => props.theme.transitions.default};

  visibility: hidden;
  position: absolute;
  text-align: center;
  z-index: 150;
  // bottom: ${(props) => props.vertical_position};
  top: ${(props) => props.vertical_position};
  ${(props) => props.flip_horizontal && `right: ${props.horizontal_position};`}
  ${(props) => !props.flip_horizontal && `left: ${props.horizontal_position};`}
  opacity: 0;
`;

const TooltipContainer = styled.span`
  position: relative;
  &:hover ${TooltipBox} {
    transition-delay: ${(props) =>
      props.specified_delay ||
      props.theme.components.tooltip.transition_delay}ms;
    visibility: visible;
    opacity: 0.9;
  }
`;

const Tooltip = ({
  text,
  children,
  vertical_position = "100%",
  specified_delay = null,
  style = {},
  flip_horizontal = false,
  horizontal_position = "50%",
}) => {
  return (
    <TooltipContainer specified_delay={specified_delay} style={style}>
      <TooltipBox
        vertical_position={vertical_position}
        flip_horizontal={flip_horizontal}
        horizontal_position={horizontal_position}
      >
        {text}
      </TooltipBox>
      {children}
    </TooltipContainer>
  );
};

export default Tooltip;
