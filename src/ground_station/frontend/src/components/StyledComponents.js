import styled from 'styled-components';

export const Button = styled.button`
  font-size: ${(props) => props.theme.fonts.button.size};
  font-family: ${(props) => props.theme.fonts.button.family};
  font-weight: ${(props) => props.theme.fonts.button.weight};
  font-style: ${(props) => props.theme.fonts.button.style};

  background: ${(props) => props.theme.primary};
  color: ${(props) => props.theme.on_primary};
  border: ${(props) => props.theme.components.button.border_style};
  max-width: ${(props) => props.theme.components.button.max_width};
  border-radius: ${(props) => props.theme.components.button.border_radius};
  padding: 0em 0.7em;
  transition: ${(props) => props.theme.transitions.default};
  font-family: ${(props) => props.theme.fonts.title};

  &:hover {
    cursor: pointer;
    background: ${(props) => props.theme.primary_hover};
    box-shadow: ${(props) => props.theme.components.button.hover_shadow};
  }

  &:active {
    filter: brightness(0.9);
  }
`;

export const WarningButton = styled(Button)`
  background: ${(props) => props.theme.error};
  color: ${(props) => props.theme.on_error};

  &:hover {
    background: ${(props) => props.theme.error_hover};
  }
`;

export const NeutralButton = styled(Button)`
  background: ${(props) => props.theme.surface_hover_hard};
  color: ${(props) => props.theme.on_surface};

  &:hover {
    background: ${(props) => props.theme.surface_hover};
  }
`;

export const SaveButton = styled(Button)`
  background: ${(props) =>
    props.saved === 'yes'
      ? props.theme.surface_hover_hard
      : props.theme.primary};
  color: ${(props) =>
    props.saved === 'yes' ? props.theme.surface : props.theme.on_primary};
  &:hover {
    background: ${(props) =>
      props.saved === 'yes'
        ? props.theme.surface_hover_hard
        : props.theme.primary_hover};
    cursor: ${(props) => (props.saved === 'yes' ? 'default' : 'pointer')};
    box-shadow: ${(props) =>
      props.saved === 'yes'
        ? 'none'
        : props.theme.components.button.hover_shadow};
  }
  &:active {
    filter: ${(props) => (props.saved === 'yes' ? 'none' : 'brightness(0.9)')};
  }
`;

export const Input = styled.input`
  font-size: ${(props) => props.theme.fonts.input.size};
  font-family: ${(props) => props.theme.fonts.input.family};
  font-weight: ${(props) => props.theme.fonts.input.weight};
  font-style: ${(props) => props.theme.fonts.input.style};

  background: ${(props) => props.theme.surface_alt};
  color: ${(props) => props.theme.on_surface};

  border: 0px solid ${(props) => props.theme.surface_hover};
  border-radius: ${(props) => props.theme.components.input.border_radius};
  padding: ${(props) => props.theme.components.input.padding_top_bottom} 0em;
  padding-left: ${(props) => props.theme.components.input.padding_sides};
  width: ${(props) => props.theme.components.input.width};
  height: ${(props) => props.theme.components.input.height};

  transition: ${(props) => props.theme.transitions.default};

  &:hover {
    cursor: text;
    box-shadow: ${(props) => props.theme.components.input.hover_shadow};
  }
  &:focus {
    outline: 1px solid ${(props) => props.theme.surface_hover_hard};
  }
`;

export const Select = styled.select`
  font-size: ${(props) => props.theme.fonts.input.size};
  font-family: ${(props) => props.theme.fonts.input.family};
  font-weight: ${(props) => props.theme.fonts.input.weight};
  font-style: ${(props) => props.theme.fonts.input.style};

  background: ${(props) => props.theme.surface_alt};
  color: ${(props) => props.theme.on_surface};

  border: 0px solid ${(props) => props.theme.surface_hover};
  border-radius: ${(props) => props.theme.components.input.border_radius};
  padding: ${(props) => props.theme.components.input.padding_top_bottom} 0em;
  padding-left: ${(props) => props.theme.components.input.padding_sides};
  width: ${(props) => props.theme.components.input.width};

  box-sizing: content-box;

  transition: ${(props) => props.theme.transitions.default};

  &:hover {
    cursor: pointer;
    box-shadow: ${(props) => props.theme.components.input.hover_shadow};
  }
  &:focus {
    outline: 1px solid ${(props) => props.theme.surface_hover_hard};
  }
`;

export const Option = styled.option`
  background: ${(props) => props.theme.surface_alt};
  color: ${(props) => props.theme.on_surface};

  border: 0px solid ${(props) => props.theme.surface_hover};
  border-radius: ${(props) => props.theme.components.input.border_radius};
  padding: ${(props) => props.theme.components.input.padding_top_bottom} 0em;
  padding-left: ${(props) => props.theme.components.input.padding_sides};
  width: ${(props) => props.theme.components.input.width};

  box-sizing: content-box;

  transition: ${(props) => props.theme.transitions.default};

  &:hover {
    cursor: pointer;
    box-shadow: ${(props) => props.theme.components.input.hover_shadow};
  }
  &:focus {
    outline: 1px solid ${(props) => props.theme.surface_hover_hard};
  }
`;

const SwitchLabel = styled.label`
  cursor: pointer;
  display: inline-block;
`;

const StyledSwitch = styled.div`
  position: relative;
  margin: ${(props) => props.theme.components.switch.margin};
  width: ${(props) => props.theme.components.switch.width};
  height: ${(props) => props.theme.components.switch.height};
  background: ${(props) => props.theme.surface_container_highest};
  border-radius: ${(props) => props.theme.components.switch.border_radius};
  padding: 4px;
  border: ${(props) => props.theme.components.switch.border_style}
    ${(props) => props.theme.outline};
  &:before {
    transition: ${(props) => props.theme.transitions.default};
    content: '';
    position: absolute;
    width: ${(props) => props.theme.components.switch.handle_width};
    height: ${(props) => props.theme.components.switch.handle_height};
    border-radius: ${(props) =>
      props.theme.components.switch.handle_border_radius};
    color: ${(props) => props.theme.outline};
    top: 50%;
    left: 3px;
    background: white;
    transform: translate(1px, -50%);
  }
`;

const StyledInput = styled.input`
  opacity: 0;
  position: absolute;

  &:checked + ${StyledSwitch} {
    background: ${(props) => props.theme.primary};

    &:before {
      transform: translate(
        calc(
          ${(props) => props.theme.components.switch.width} -
            ${(props) => props.theme.components.switch.handle_width}
        ),
        -50%
      );
    }
  }
`;

export function Switch({ checked, setChecked, onChange, defaultChecked }) {
  const handleChange = () => {
    setChecked(!checked);
  };
  return (
    <SwitchLabel>
      <StyledInput
        type="checkbox"
        checked={checked}
        onChange={onChange != null ? onChange : handleChange}
        defaultChecked={defaultChecked}
      />
      <StyledSwitch />
    </SwitchLabel>
  );
}
