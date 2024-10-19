import React from "react";
import { Line } from "react-chartjs-2";
import Chart, { scales } from "chart.js/auto";

import {
  CardSectionTitle,
  CardRow,
  CardBreak,
  LabelDataPair,
} from "../core/PageParts";
import { UseGenericGetApi } from "../api_interface/ggs_api";
import { StyTable, StyTableRow, StyTableCell } from "./styled/StyledTable";

import { SelectMenu, StyButton } from "./styled/StyledComponents";

function unixTimeToShortUtc(unix_time) {
  let date = new Date(unix_time * 1000);
  return date.toISOString().split("T")[1].split(".")[0] + " UTC";
}

function clockDisplay(seconds) {
  const data = {
    days: Math.floor(seconds / 86400),
    hours: Math.floor(seconds / 3600),
    minutes: Math.floor((seconds % 3600) / 60),
    seconds: Math.floor(seconds % 60),
  };

  let output = "";
  if (data.days > 0) {
    output += `${data.days}d `;
  }

  if (data.hours > 0) {
    output += `${data.hours}h `;
  }

  if (data.minutes > 0) {
    output += `${data.minutes}m `;
  }

  output += `${data.seconds}s`;

  return output;
}

function CurrentTelemetryDisplay({ latest_packet }) {
  const [age, setAge] = React.useState();

  React.useEffect(() => {
    const UPDATE_INTERVAL = 1000;

    const interval = setInterval(() => {
      const now = new Date().getTime() / 1000;
      const seconds = parseInt(now - latest_packet.timestamp);
      setAge(clockDisplay(seconds));
    }, UPDATE_INTERVAL);
    return () => clearInterval(interval);
  }, [latest_packet.timestamp]);

  const TableStyle = {
    width: "100%",
    tableLayout: "fixed",
  };
  const CellStyle = {
    textAlign: "center",
  };

  return (
    <div>
      <StyTable
        style={{
          width: "100%",
        }}
      >
        <StyTableRow style={CellStyle}>
          <th>SEQ</th>
          <StyTableCell>{latest_packet.sequence_number}</StyTableCell>
          <th style={CellStyle}>Age</th>
          <StyTableCell>{age}</StyTableCell>
        </StyTableRow>
      </StyTable>
      <StyTable
        style={{
          width: "100%",
        }}
      >
        <StyTableRow>
          <th style={CellStyle}>Comment</th>
          <StyTableCell
            style={{
              textAlign: "left",
              width: "100%",
            }}
          >
            {latest_packet.comment}
          </StyTableCell>
        </StyTableRow>
      </StyTable>

      <StyTable style={TableStyle}>
        <thead>
          <StyTableRow>
            <th>A1</th>
            <th>A2</th>
            <th>A3</th>
            <th>A4</th>
            <th>A5</th>
          </StyTableRow>
        </thead>
        <tbody>
          <StyTableRow>
            <StyTableCell style={CellStyle}>{latest_packet.a1}</StyTableCell>
            <StyTableCell style={CellStyle}>{latest_packet.a2}</StyTableCell>
            <StyTableCell style={CellStyle}>{latest_packet.a3}</StyTableCell>
            <StyTableCell style={CellStyle}>{latest_packet.a4}</StyTableCell>
            <StyTableCell style={CellStyle}>{latest_packet.a5}</StyTableCell>
          </StyTableRow>
        </tbody>
      </StyTable>
    </div>
  );
}

function RawDataLineChart({ raw_data_packets }) {
  if (!raw_data_packets) {
    return <div>No data</div>;
  }

  const Data = {
    labels: raw_data_packets.map((data) => {
      return data.timestamp;
    }),
    datasets: [
      {
        label: "Sequence Number",
        data: raw_data_packets.map((data) => data.sequence_number),
        showLine: false,
        hidden: true,
      },
      {
        label: "A1",
        data: raw_data_packets.map((data) => data.a1),
      },
      {
        label: "A2",
        data: raw_data_packets.map((data) => data.a2),
      },
      {
        label: "A3",
        data: raw_data_packets.map((data) => data.a3),
      },
      {
        label: "A4",
        data: raw_data_packets.map((data) => data.a4),
      },
      {
        label: "A5",
        data: raw_data_packets.map((data) => data.a5),
      },
    ],
  };

  const options = {
    animation: false,
    scales: {
      y: {
        beginAtZero: true,
        max: 255,
      },
      x: {
        display: false,
        reverse: true,
      },
    },
    interaction: {
      intersect: false,
      mode: "index",
      axis: "x",
    },
    plugins: {
      legend: {
        display: false,
        position: "top",
      },
      tooltip: {
        callbacks: {
          title: function (tooltipItems) {
            const time = new Date(parseInt(tooltipItems[0].label) * 1000);
            return time.toISOString().split("T")[1].split(".")[0] + " UTC";
          },
          afterTitle: function (tooltipItems) {
            const time = new Date(parseInt(tooltipItems[0].label) * 1000);
            return time.toLocaleTimeString() + " Local";
          },
          // label: function (tooltipItem) {
          // return tooltipItem.dataset.label;
          // },
          footer: function (tooltipItems) {
            return "test";
          },
        },
      },
    },
  };

  return <Line data={Data} options={options} />;
}

export default function AprsTelemetryData() {
  const [range, setRange] = React.useState("Custom");
  const [apiEndpoint, setApiEndpoint] = React.useState(
    "/api/gdl/aprs_telemetry?start_time=0&end_time=1729327810&limit=50"
  );
  const { data, isLoading, error, forceUpdate } = UseGenericGetApi(apiEndpoint);
  const [latestPacket, setLatestPacket] = React.useState({
    timestamp: 0,
    a1: "unk",
    a2: "unk",
    a3: "unk",
    a4: "unk",
    a5: "unk",
    b1: "unk",
    b2: "unk",
    b3: "unk",
    b4: "unk",
    b5: "unk",
    b6: "unk",
    b7: "unk",
    b8: "unk",
    sequence_number: "unk",
    comment: "unk",
  });
  const [packets, setPackets] = React.useState([]);
  const [timeOfLastPacket, setTimeOfLastPacket] = React.useState("unknown");
  const [numPacketsOnChart, setNumPacketsOnChart] = React.useState(0);
  const [numPacketsInDatabase, setNumPacketsInDatabase] = React.useState(0);

  React.useEffect(() => {
    // console.log("Range: ", range);
    let now = new Date();
    let start_time = 0; // Beginning of time
    let end_time = now.getTime() / 1000; // current unix time in seconds
    switch (range) {
      case "5m":
        start_time = end_time - 5 * 60;
        break;
      case "10m":
        start_time = end_time - 10 * 60;
        break;
      case "30m":
        start_time = end_time - 30 * 60;
        break;
      case "1h":
        start_time = end_time - 60 * 60;
        break;
      case "Custom": // timestamp above is already set to now
        break; // todo
      default:
        break;
    }

    setApiEndpoint(
      `/api/gdl/aprs_telemetry?start_time=${start_time}&end_time=${end_time}&limit=50`
    );
  }, [range]);

  React.useEffect(() => {
    if (data && typeof data === "object") {
      setPackets(data.packets);
      setLatestPacket(data.metadata.latest_packet);
      setNumPacketsOnChart(data.metadata.num_packets_found);
      setNumPacketsInDatabase(data.metadata.num_packets_in_database);
      setTimeOfLastPacket(
        unixTimeToShortUtc(data.metadata.latest_packet.timestamp)
      );
    }
  }, [data]);

  if (isLoading) {
    return <div>Loading...</div>;
  }

  if (error) {
    return <div>Error: {error}</div>;
  }

  return (
    <div>
      <CardSectionTitle>Current Telemetry Data</CardSectionTitle>

      <LabelDataPair
        style={{
          paddingTop: "1rem",
          paddingBottom: "1rem",
        }}
        data={[
          {
            key: "Time Of Last Packet",
            value: timeOfLastPacket,
            tooltip:
              new Date(latestPacket.timestamp * 1000).toLocaleTimeString() +
              " Local",
          },
        ]}
      />
      <CurrentTelemetryDisplay latest_packet={latestPacket} />
      <CardBreak />
      <CardSectionTitle>Raw Values</CardSectionTitle>
      <RawDataLineChart raw_data_packets={packets} />
      <div
        style={{
          display: "flex",
          alignItems: "flex-end",
          flexWrap: "wrap",
          gap: "10px",
          margin: "1rem",
          justifyContent: "center",
        }}
      >
        <SelectMenu
          options={["5m", "10m", "30m", "1h", "Custom"]}
          selected={range}
          setSelected={setRange}
          style={{ flexGrow: 1, padding: "0.4rem" }}
        />
        <StyButton
          onClick={() => {
            console.log("Refresh");
            forceUpdate();
          }}
          style={{ flexGrow: 1, padding: "0.2rem" }}
        >
          Refresh
        </StyButton>
        <StyButton
          onClick={() => {
            console.log("Refresh");
            forceUpdate();
          }}
          style={{ flexGrow: 1, padding: "0.2rem" }}
        >
          Refresh
        </StyButton>
      </div>

      <LabelDataPair
        data={[
          {
            key: "Time Of Last Packet",
            value: timeOfLastPacket,
            tooltip:
              new Date(latestPacket.timestamp * 1000).toLocaleTimeString() +
              " Local",
          },
          {
            key: "",
            value:
              clockDisplay(
                new Date().getTime() / 1000 - latestPacket.timestamp
              ) + " ago",
            tooltip: "b",
          },
          {
            key: "Packets On Chart",
            value: numPacketsOnChart,
            tooltip: "c",
          },
          {
            key: "Packets In Database",
            value: numPacketsInDatabase,
            tooltip:
              "The total number of data packets that are in the database",
          },
        ]}
      />
    </div>
  );
}
