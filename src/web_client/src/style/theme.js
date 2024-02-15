import { createGlobalStyle, ThemeProvider } from "styled-components";
import React from "react";

import {FontFiles, StyleFonts} from "./fonts";
import StyleStructure from "./structure"
import { lightTheme2, darkTheme2 } from "./colors"
import { GwsGlobal } from '../GlobalContext';


const GlobalStyle = createGlobalStyle`
  body {
    background: ${props => props.theme.background};
    font-family: ${props => props.theme.fonts.default.family};
    padding: 0;
    margin: 0;
  }
`

const providedTheme = {
  ...StyleStructure,
  fonts: {
    ...StyleFonts,
  },
};

export function AppStyle({ children }) {
  const { clientDarkTheme } = React.useContext(GwsGlobal);
  return (
    <>
      <FontFiles />
      <ThemeProvider theme={providedTheme}>
        <ThemeProvider theme={clientDarkTheme ? darkTheme2 : lightTheme2}>
          <GlobalStyle />
          {children}
        </ThemeProvider>
      </ThemeProvider>
    </>
  )
}