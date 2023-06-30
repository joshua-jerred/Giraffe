import styled from 'styled-components';

export const Page = styled.div`
  padding: 0 0px;
  color: ${(props) => props.theme.on_surface};
  max-width: 2500px;
  margin: 0 ${(props) => props.theme.pages.side_margin};
`;

export const PageTitle = styled.div`
  font-size: ${(props) => props.theme.fonts.page_title.size};
  font-family: ${(props) => props.theme.fonts.page_title.family};
  font-weight: ${(props) => props.theme.fonts.page_title.weight};
  font-style: ${(props) => props.theme.fonts.page_title.style};

  text-align: center;
  color: ;
  width: 100%;
  padding: 0;
  margin-top: ${(props) => props.theme.pages.title_margin_top};
`;

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
  margin-bottom: 50px;
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

export const CardMasonryLayout = styled.div`
  columns: 4 400px;
  column-gap: 1rem;

  & > ${CardStyle} {
    width: 500px;
    display: inline-block;
    width: 100%;
    text-align: center;
    font-family: system-ui;
    font-weight: 900;
    font-size: 2rem;
  }
`;

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
