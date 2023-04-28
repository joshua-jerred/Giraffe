import { createGlobalStyle, ThemeProvider } from "styled-components";

import {FontFiles, StyleFonts} from "./fonts";
import StyleStructure from "./structure"
import { lightTheme2, darkTheme2 } from "./colors"


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

export function AppStyle({ darkMode, children }) {
  return (
    <>
      <FontFiles />
      <ThemeProvider theme={providedTheme}>
        <ThemeProvider theme={darkMode ? darkTheme2 : lightTheme2}>
          <GlobalStyle />
          {children}
        </ThemeProvider>
      </ThemeProvider>
    </>
  )
}