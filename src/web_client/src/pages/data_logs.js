import styled from "styled-components";

import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";
import { DataBlock } from "../components/DataBlock";
import { UseGenericGetApi } from "../api_interface/ggs_api";
import Tooltip from "../components/Tooltip";

const GgsLogStyle = styled.ul`
  list-style-type: none;
  margin: 0;
  padding: 0;
  width: 100%;

  li {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
    margin-bottom: 0.5em;
    overflow: hidden;
    border-bottom: 1px solid #ccc;

    .sp1 {
      font-weight: bold;
      margin-right: 0.5em;
      font-size: 0.8em;
    }
  }
`;

function GgsLog() {
  const UPDATE_INTERVAL_MS = 5000;
  const { data, isLoading, error } = UseGenericGetApi(
    "/api/ggs/log",
    UPDATE_INTERVAL_MS
  );

  if (error) {
    return <p>Error: {error}</p>;
  }

  return (
    <div>
      <GgsLogStyle>
        {data &&
          data.map((item) => (
            <Tooltip
              text={new Date(item.timestamp * 1000).toUTCString()}
              specified_delay={50}
              key={item.timestamp}
              vertical_position="50px"
            >
              <li key={item.id}>
                <span className="sp1">{item.level} </span>
                <span>{item.message}</span>
              </li>
            </Tooltip>
          ))}
        {isLoading === true ? "loading" : "loaded"}
      </GgsLogStyle>
    </div>
  );
}

function DataLogsPage() {
  return (
    <>
      <PageTitle title={"Data Logs"}>@todo - add a description here</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="Ground Station Status">
            <DataBlock resource="ggs" category="status" stream_name="status" />
          </Card>
          <Card title="Ground Station Log">
            <GgsLog />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default DataLogsPage;
