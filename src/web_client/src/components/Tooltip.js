import React from 'react';
import styled from 'styled-components';

const TooltipBox = styled.div`
	color: ${(props) => props.theme.tooltip_text};
	background-color: ${(props) => props.theme.tooltip_background};

	max-width: ${(props) => props.theme.components.tooltip.max_width};
	min-width: ${(props) => props.theme.components.tooltip.min_width};
	border-radius: ${(props) => props.theme.components.tooltip.border_radius};
	padding: ${(props) => props.theme.components.tooltip.padding};
	transition: ${(props) => props.theme.transitions.default};

	visibility: hidden;
	position: absolute;
	text-align: center;
	z-index: 101;
	bottom: 125%;
	opacity: 0;
`;

const TooltipContainer = styled.span`
	position: relative;
	width: 100%;
	&:hover ${TooltipBox} {
		transition-delay: ${(props) => props.theme.components.tooltip.transition_delay};
		visibility: visible;
		opacity: 1;
	}
`;

const Tooltip = ({ text, children, position }) => {
	return (
		<TooltipContainer>
			{children}
			<TooltipBox>{text}</TooltipBox>
		</TooltipContainer>
	);
};

export default Tooltip;
