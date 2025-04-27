import ReactMarkdown from 'react-markdown'

export function Markdown ({children}) {  
  return (
    <ReactMarkdown>
      {children}
    </ReactMarkdown>
  );
}