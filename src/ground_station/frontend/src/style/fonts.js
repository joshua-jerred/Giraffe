import { createGlobalStyle } from "styled-components";

// Roboto
import RobotoThin from "./fonts/roboto/roboto-thin.woff2";
import RobotoThinItalic from "./fonts/roboto/roboto-thinitalic.woff2";
import RobotoLight from "./fonts/roboto/roboto-light.woff2";
import RobotoLightItalic from "./fonts/roboto/roboto-lightitalic.woff2";
import RobotoRegular from "./fonts/roboto/roboto-regular.woff2";
import RobotoRegularItalic from "./fonts/roboto/roboto-italic.woff2";
import RobotoMedium from "./fonts/roboto/roboto-medium.woff2";
import RobotoMediumItalic from "./fonts/roboto/roboto-mediumitalic.woff2";
import RobotoBold from "./fonts/roboto/roboto-bold.woff2";
import RobotoBoldItalic from "./fonts/roboto/roboto-bolditalic.woff2";
import RobotoBlack from "./fonts/roboto/roboto-black.woff2";
import RobotoBlackItalic from "./fonts/roboto/roboto-blackitalic.woff2";

// Roboto Mono
import RobotoMonoThin from "./fonts/roboto-mono/roboto-mono-thin.woff2";
import RobotoMonoThinItalic from "./fonts/roboto-mono/roboto-mono-thin-italic.woff2";
import RobotoExtraLight from "./fonts/roboto-mono/roboto-mono-extralight.woff2";
import RobotoExtraLightItalic from "./fonts/roboto-mono/roboto-mono-extralight-italic.woff2";
import RobotoMonoLight from "./fonts/roboto-mono/roboto-mono-light.woff2";
import RobotoMonoLightItalic from "./fonts/roboto-mono/roboto-mono-light-italic.woff2";
import RobotoMonoRegular from "./fonts/roboto-mono/roboto-mono-regular.woff2";
import RobotoMonoRegularItalic from "./fonts/roboto-mono/roboto-mono-italic.woff2";
import RobotoMonoMedium from "./fonts/roboto-mono/roboto-mono-medium.woff2";
import RobotoMonoMediumItalic from "./fonts/roboto-mono/roboto-mono-medium-italic.woff2";
import RobotoMonoBold from "./fonts/roboto-mono/roboto-mono-bold.woff2";
import RobotoMonoBoldItalic from "./fonts/roboto-mono/roboto-mono-bold-italic.woff2";

// Roboto Slab
import RobotoSlabThin from "./fonts/roboto-slab/roboto-slab-thin.woff2";
import RobotoSlabExtraLight from "./fonts/roboto-slab/roboto-slab-extralight.woff2";
import RobotoSlabLight from "./fonts/roboto-slab/roboto-slab-light.woff2";
import RobotoSlabRegular from "./fonts/roboto-slab/roboto-slab-regular.woff2";
import RobotoSlabMedium from "./fonts/roboto-slab/roboto-slab-medium.woff2";
import RobotoSlabSemiBold from "./fonts/roboto-slab/roboto-slab-semibold.woff2";
import RobotoSlabBold from "./fonts/roboto-slab/roboto-slab-bold.woff2";
import RobotoSlabExtraBold from "./fonts/roboto-slab/roboto-slab-extrabold.woff2";
import RobotoSlabBlack from "./fonts/roboto-slab/roboto-slab-black.woff2";

export const FontFiles = createGlobalStyle`

  @font-face {
    font-family: 'Roboto';
    src: url(${RobotoThin}) format('woff2');
    font-weight: 100;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto';
    src: url(${RobotoThinItalic}) format('woff2');
    font-weight: 100;
    font-style: italic;
  }
  @font-face {
    font-family: 'Roboto';
    src: url(${RobotoLight}) format('woff2');
    font-weight: 300;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto';
    src: url(${RobotoLightItalic}) format('woff2');
    font-weight: 300;
    font-style: italic;
  }
  @font-face {
    font-family: 'Roboto';
    src: url(${RobotoRegular}) format('woff2');
    font-weight: 400;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto';
    src: url(${RobotoRegularItalic}) format('woff2');
    font-weight: 400;
    font-style: italic;
  }
  @font-face {
    font-family: 'Roboto';
    src: url(${RobotoMedium}) format('woff2');
    font-weight: 500;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto';
    src: url(${RobotoMediumItalic}) format('woff2');
    font-weight: 500;
    font-style: italic;
  }
  @font-face {
    font-family: 'Roboto';
    src: url(${RobotoBold}) format('woff2');
    font-weight: 700;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto';
    src: url(${RobotoBoldItalic}) format('woff2');
    font-weight: 700;
    font-style: italic;
  }
  @font-face {
    font-family: 'Roboto';
    src: url(${RobotoBlack}) format('woff2');
    font-weight: 900;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto';
    src: url(${RobotoBlackItalic}) format('woff2');
    font-weight: 900;
    font-style: italic;
  }

  @font-face {
    font-family: 'Roboto Mono';
    src: url(${RobotoMonoThin}) format('woff2');
    font-weight: 100;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto Mono';
    src: url(${RobotoMonoThinItalic}) format('woff2');
    font-weight: 100;
    font-style: italic;
  }
  @font-face {
    font-family: 'Roboto Mono';
    src: url(${RobotoExtraLight}) format('woff2');
    font-weight: 200;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto Mono';
    src: url(${RobotoExtraLightItalic}) format('woff2');
    font-weight: 200;
    font-style: italic;
  }
  @font-face {
    font-family: 'Roboto Mono';
    src: url(${RobotoMonoLight}) format('woff2');
    font-weight: 300;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto Mono';
    src: url(${RobotoMonoLightItalic}) format('woff2');
    font-weight: 300;
    font-style: italic;
  }
  @font-face {
    font-family: 'Roboto Mono';
    src: url(${RobotoMonoRegular}) format('woff2');
    font-weight: 400;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto Mono';
    src: url(${RobotoMonoRegularItalic}) format('woff2');
    font-weight: 400;
    font-style: italic;
  }
  @font-face {
    font-family: 'Roboto Mono';
    src: url(${RobotoMonoMedium}) format('woff2');
    font-weight: 500;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto Mono';
    src: url(${RobotoMonoMediumItalic}) format('woff2');
    font-weight: 500;
    font-style: italic;
  }
  @font-face {
    font-family: 'Roboto Mono';
    src: url(${RobotoMonoBold}) format('woff2');
    font-weight: 700;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto Mono';
    src: url(${RobotoMonoBoldItalic}) format('woff2');
    font-weight: 700;
    font-style: italic;
  }

  @font-face {
    font-family: 'Roboto Slab';
    src: url(${RobotoSlabThin}) format('woff2');
    font-weight: 100;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto Slab';
    src: url(${RobotoSlabExtraLight}) format('woff2');
    font-weight: 100;
    font-style: italic;
  }
  @font-face {
    font-family: 'Roboto Slab';
    src: url(${RobotoSlabLight}) format('woff2');
    font-weight: 300;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto Slab';
    src: url(${RobotoSlabRegular}) format('woff2');
    font-weight: 400;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto Slab';
    src: url(${RobotoSlabMedium}) format('woff2');
    font-weight: 500;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto Slab';
    src: url(${RobotoSlabBold}) format('woff2');
    font-weight: 700;
    font-style: normal;
  }
  @font-face {
    font-family: 'Roboto Slab';
    src: url(${RobotoSlabBlack}) format('woff2');
    font-weight: 900;
    font-style: normal;
  }
`;

export const StyleFonts = {
  default: {
    family: `Roboto Mono`,
    weight: `400`,
    size: `1rem`,
    style: `normal`,
  },

  // !! Anything that must be mono should use this. The default may change.
  mono: {
    family: `Roboto Mono`,
    weight: `400`,
    size: `1rem`,
    style: `normal`,
  },

  // Primary
  page_title: {
    family: `Roboto Slab`,
    weight: `500`,
    size: `1.75rem`,
    style: `normal`,
  },
  page_title_block_content: {
    family: `Roboto`,
    weight: `400`,
    size: `1rem`,
    style: `normal`,
  },
  card_title: {
    family: `Roboto Mono`,
    weight: `400`,
    size: `1.4rem`,
    style: `normal`,
  },
  card_section_title: {
    family: `Roboto Mono`,
    weight: `400`,
    size: `1.25rem`,
    style: `normal`,
  },
  card_body: {
    family: `Roboto Mono`,
    weight: `300`,
    size: `1rem`,
    style: `normal`,
  },
  small_paragraph: {
    family: `Roboto Mono`,
    weight: `300`,
    size: `0.9rem`,
    style: `normal`,
  },

  // Auxiliary
  nav_bar_link_label: {
    // That 3 letter label
    family: `Roboto Mono`,
    weight: `400`,
    size: `1.1rem`,
    style: `normal`,
  },
  nav_bar_link_title: {
    family: `Roboto Mono`,
    weight: `300`,
    size: `1.0rem`,
    style: `normal`,
  },
  nav_bar_content: {},
  status_bar: {
    family: `Roboto Mono`,
    weight: `700`,
    size: `1rem`,
    style: `normal`,
  },
  button: {
    family: `Roboto Mono`,
    weight: `500`,
    size: `18px`,
    style: `normal`,
  },
  input: {
    family: `Roboto Mono`,
    weight: `400`,
    size: `15px`,
    style: `normal`,
  },
};
