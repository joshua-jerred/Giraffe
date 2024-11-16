import styled from "styled-components";

export const StyButton = styled.button`
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

  white-space: nowrap;

  &:hover {
    cursor: pointer;
    background: ${(props) => props.theme.primary_hover};
    box-shadow: ${(props) => props.theme.components.button.hover_shadow};
  }

  &:active {
    filter: brightness(0.9);
  }

  &:disabled {
    cursor: not-allowed;
    filter: brightness(0.5);

    &:hover {
      box-shadow: none;
    }
  }
`;

export const StyWarningButton = styled(StyButton)`
  background: ${(props) => props.theme.error};
  color: ${(props) => props.theme.on_error};

  &:hover {
    background: ${(props) => props.theme.error_hover};
  }

  &:disabled {
    background: ${(props) => props.theme.error_disabled};
    color: ${(props) => props.theme.on_error_disabled};
    cursor: not-allowed;

    &:active {
      filter: brightness(1);
    }

    &:hover {
      box-shadow: none;
    }
  }
`;

export const StyNeutralButton = styled(StyButton)`
  background: ${(props) => props.theme.surface_hover_hard};
  color: ${(props) => props.theme.on_surface};

  &:hover {
    background: ${(props) => props.theme.surface_hover};
  }
`;

export const StySaveButton = styled(StyButton)`
  background: ${(props) =>
    props.saved === "yes"
      ? props.theme.surface_hover_hard
      : props.theme.primary};
  color: ${(props) =>
    props.saved === "yes" ? props.theme.surface : props.theme.on_primary};
  &:hover {
    background: ${(props) =>
      props.saved === "yes"
        ? props.theme.surface_hover_hard
        : props.theme.primary_hover};
    cursor: ${(props) => (props.saved === "yes" ? "default" : "pointer")};
    box-shadow: ${(props) =>
      props.saved === "yes"
        ? "none"
        : props.theme.components.button.hover_shadow};
  }
  &:active {
    filter: ${(props) => (props.saved === "yes" ? "none" : "brightness(0.9)")};
  }
`;

export const StyInput = styled.input`
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

  &:disabled {
    cursor: not-allowed;
    filter: brightness(0.5);

    &:hover {
      box-shadow: none;
    }
  }
`;

export const StySelect = styled.select`
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

export const StyOption = styled.option`
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

export function SelectMenu({ options, selected, setSelected, style = {} }) {
  return (
    <StySelect
      value={selected}
      onChange={(e) => setSelected(e.target.value)}
      name="select"
      style={style}
    >
      {options.map((option) => (
        <StyOption key={option} value={option}>
          {option}
        </StyOption>
      ))}
    </StySelect>
  );
}

const StySwitchLabel = styled.label`
  cursor: pointer;
  display: inline-block;
`;

const StySwitch = styled.div`
  position: relative;
  margin: ${(props) => props.theme.components.switch.margin};
  width: ${(props) => props.theme.components.switch.width};
  height: ${(props) => props.theme.components.switch.height};
  background: ${(props) => props.offColor || props.theme.surface};
  border-radius: ${(props) => props.theme.components.switch.border_radius};
  padding: 4px;
  border: ${(props) => props.theme.components.switch.border_style}
    ${(props) => props.theme.outline};
  &:before {
    transition: ${(props) => props.theme.transitions.default};
    content: "";
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

const StySwitchInput = styled.input`
  opacity: 0;
  position: absolute;

  &:checked + ${StySwitch} {
    background: ${(props) =>
      props.onColor ? props.onColor : props.theme.primary};

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

export function Switch({
  checked,
  setChecked,
  onChange,
  defaultChecked,
  onColor = null,
  offColor = null,
}) {
  const handleChange = () => {
    setChecked(!checked);
  };
  return (
    <StySwitchLabel>
      <StySwitchInput
        type="checkbox"
        checked={checked}
        onChange={onChange != null ? onChange : handleChange}
        defaultChecked={defaultChecked}
        onColor={onColor}
        offColor={offColor}
      />
      <StySwitch onColor={onColor} offColor={offColor} />
    </StySwitchLabel>
  );
}

const SwitchWithLabelContainer = styled.div`
  display: flex;
  align-items: center;
  justify-content: center;
  margin: 0.2em 0;
`;

export function SwitchWithLabel({
  checked,
  setChecked,
  onChange,
  defaultChecked,
  label,
  onColor = null,
  offColor = null,
}) {
  return (
    <SwitchWithLabelContainer>
      <label>{label}</label>
      <Switch
        checked={checked}
        setChecked={setChecked}
        onChange={onChange}
        defaultChecked={defaultChecked}
        onColor={onColor}
        offColor={offColor}
      />
    </SwitchWithLabelContainer>
  );
}
