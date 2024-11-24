const StyleStructure = {
  transitions: {
    default: "all 0.1s linear",
    nav_bar: "all 0.1s linear",
  },
  nav: {
    width: {
      expanded: "200px",
      collapsed: "50px",
    },
    hover_margin: "10px",
  },
  status_bar: {
    height: "50px",
    padding: "10px",
    border_radius: "15px",
  },
  pages: {
    side_margin: "25px",
    max_width: "95rem",
    title_margin: "15px",
  },
  cards: {
    padding: "15px",
    margin_v: "0px",
    margin_h: "0px",
    border_style: `1px solid`,
    border_radius: "10px",
    // title_size: "10em",
    title_border_bottom: "1px solid",
    card_break_border: "1px solid",
    card_break_width: "90%",
    card_break_margin: "1.5em",
  },
  components: {
    button: {
      border_radius: "5px",
      max_width: "25rem",
      border_style: "0px solid",
      hover_shadow: "0px 0px 4px 0px rgba(0,0,0,0.75)",
    },
    input: {
      hover_shadow: "0px 0px 2px 0px rgba(0,0,0,0.75)",
      border_radius: "3px",
      width: "190px",
      height: "30px",
      padding_top_bottom: "5px",
      padding_sides: "10px", // Left only on text inputs
      focus_outline: "1px solid",
    },
    markdown: {},
    switch: {
      width: "30px",
      margin: "4px",
      height: "12px",
      border_radius: "15px",
      border_style: "1px solid",
      handle_width: "15px",
      handle_height: "15px",
      handle_border_radius: "10px",
    },
    tooltip: {
      max_width: "500px",
      min_width: "100px",
      max_height: "100px",
      border_radius: "5px",
      padding: "0.5em 0.7em",
      transition_delay: "500",
    },
  },
};
export default StyleStructure;
