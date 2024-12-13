import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";
import { useApiGetData } from "../api_interface/ggs_api";
import { DataBlock } from "../components/DataBlock";
import { useEffect, useState } from "react";
import { ApiRequestButton } from "../components/api_request_button";
import Tooltip from "../components/Tooltip";

const DiagnosticMetadata = require("giraffe-protocol").DiagnosticMetadata;
const BuildIdToMetadataMap = () => {
  const map = {};
  for (const category in DiagnosticMetadata) {
    for (const item_label in DiagnosticMetadata[category]) {
      const item = DiagnosticMetadata[category][item_label];
      map[item.value] = item;
    }
  }
  // console.log("DiagnosticMetadataMap", map);
  return map;
};
const DiagnosticMetadataMap = BuildIdToMetadataMap();

function ErrorFrameItem({ error_number }) {
  let error_name = ""; // DiagnosticMetadataMap[`0x${error_number}`].name

  if (DiagnosticMetadataMap[`0x${error_number}`]) {
    error_name = DiagnosticMetadataMap[`0x${error_number}`].name;
  } else {
    error_name = "Unknown Error";
  }

  return (
    <div>
      <span>
        Error {error_number} - {error_name}
      </span>
    </div>
  );
}

function ErrorFrameList({ items }) {
  return (
    <div>
      {items.map((item, index) => (
        <ErrorFrameItem key={index} error_number={item} />
      ))}
    </div>
  );
}

function ErrorFrame() {
  const ERROR_FRAME_UPDATE_INTERVAL = 2000;
  const {
    data: frameMetaData,
    isLoading: isFrameMetaLoading,
    error: frameMetaError,
    setNeedUpdate: setFrameMetadataNeedUpdate,
  } = useApiGetData(
    "flight_data",
    "error_frame",
    "all",
    ERROR_FRAME_UPDATE_INTERVAL
  );
  const [frameMetaReady, setFrameMetaReady] = useState(false);

  const [numActiveErrors, setNumActiveErrors] = useState(0);
  const [numTotalErrors, setNumTotalErrors] = useState(0);
  const [activeErrors, setActiveErrors] = useState(["Error 1", "Error 2"]);

  useEffect(() => {
    // Still loading
    if (isFrameMetaLoading) {
      return;
    }

    // Error
    if (frameMetaError) {
      console.error("Error loading frame metadata", frameMetaError);
      return;
    }

    if (!frameMetaData) {
      return;
    }

    const NEEDED_PROPERTIES = [
      "num_total_errors",
      "num_active_errors",
      "active_errors",
    ];
    for (let prop of NEEDED_PROPERTIES) {
      if (!frameMetaData.values.hasOwnProperty(prop)) {
        console.error(
          "Error: Missing property in error frame: ",
          prop,
          frameMetaData
        );
        return;
      }
    }
    setFrameMetaReady(true);
    setNumActiveErrors(frameMetaData.values.num_active_errors);
    setNumTotalErrors(frameMetaData.values.num_total_errors);
    setActiveErrors(frameMetaData.values.active_errors);
  }, [frameMetaData]);

  return <ErrorFrameList items={activeErrors} />;
}

function ExtensionsData() {
  const {
    data: extensionsData,
    isLoading: isExtensionsDataLoading,
    error: extensionsDataError,
    setNeedUpdate: setExtensionsDataNeedUpdate,
  } = useApiGetData("flight_data", "extensions", "all", 2000);

  const [loadingStatus, setLoadingStatus] = useState(null);
  const [lastUpdatedTimeStamp, setLastUpdatedTimeStamp] = useState("--:--");
  const [lastUpdatedSource, setLastUpdatedSource] = useState("n/a");
  const [extensions, setExtensions] = useState([]); // [{name: "ext name", status: "running"}, ...]
  const [moduleStats, setModuleStats] = useState({
    num_configured: "n/d",
    num_active: "n/d",
    num_inactive: "n/d",
  });

  useEffect(() => {
    // Still loading
    if (isExtensionsDataLoading) {
      setLoadingStatus("Loading...");
      return;
    }

    // Error
    if (extensionsDataError) {
      setLoadingStatus("Error loading data");
      console.error("Error loading extensions data", extensionsDataError);
      return;
    }

    if (!extensionsData) {
      setLoadingStatus("Error: No data");
      return;
    }

    setLoadingStatus("Loaded");

    if (!extensionsData.hasOwnProperty("values")) {
      console.error("Error: Missing values in extensions data", extensionsData);
      setLoadingStatus("Error: No values");
      return;
    }

    if (!extensionsData.hasOwnProperty("metadata")) {
      console.error(
        "Error: Missing metadata in extensions data",
        extensionsData
      );
      setLoadingStatus("Error: No metadata");
      return;
    }

    if (!extensionsData.metadata.hasOwnProperty("MS_SINCE_LAST_UPDATE")) {
      console.error(
        "Error: Missing metadata MS_SINCE_LAST_UPDATE in extensions data",
        extensionsData
      );
      setLoadingStatus("Error: No metadata MS_SINCE_LAST_UPDATE");
      return;
    }

    const NEEDED_PROPERTIES = [
      "last_updated_source",
      "extensions",
      "module_stats",
    ];
    for (let prop of NEEDED_PROPERTIES) {
      if (!extensionsData.values.hasOwnProperty(prop)) {
        console.error(
          "Error: Missing property in extensions data: ",
          prop,
          extensionsData
        );
        return;
      }
    }

    // mm:ss:ms format from ms
    let ms_since_update = extensionsData.metadata.MS_SINCE_LAST_UPDATE;
    let seconds = Math.floor(ms_since_update / 1000);
    let minutes = Math.floor(seconds / 60);
    let ms = ms_since_update % 1000;
    // console.log(ms);
    let ms_padded = ms < 10 ? `00${ms}` : ms < 100 ? `0${ms}` : `${ms}`;
    // console.log(ms_padded, typeof ms_padded);
    setLastUpdatedTimeStamp(
      `${minutes < 10 ? "0" : ""}${minutes}:${seconds % 60 < 10 ? "0" : ""}${
        seconds % 60
      }:${ms_padded}`
    );

    setLastUpdatedSource(extensionsData.values.last_updated_source);
    setExtensions(extensionsData.values.extensions);
    setModuleStats(extensionsData.values.module_stats);
  }, [extensionsData, extensionsDataError, isExtensionsDataLoading]);

  return (
    <div>
      <div>
        {moduleStats.num_configured} configured, {moduleStats.num_active}{" "}
        active, {moduleStats.num_inactive} inactive
      </div>
      <div>
        {Object.keys(extensions).map((ext, index) => (
          <div
            key={index}
            style={{
              margin: "5px",
              padding: "5px",
              border: "1px solid black",
              display: "flex",
              justifyContent: "space-around",
            }}
          >
            {ext} - {extensions[ext].name} - {extensions[ext].status}
            <div>
              <ApiRequestButton
                api_endpoint="/api/command"
                title="Restart"
                api_method="POST"
                request_data_callback={() => {
                  return {
                    command_string: `cmd/etm/rst/${ext}`,
                    send_method: "tcp_socket",
                  };
                }}
              />
              <ApiRequestButton
                api_endpoint="/api/command"
                title="Disable"
                api_method="POST"
                request_data_callback={() => {
                  return {
                    command_string: `cmd/etm/dsx/${ext}`,
                    send_method: "tcp_socket",
                  };
                }}
              />
              <ApiRequestButton
                api_endpoint="/api/command"
                title="Enable"
                api_method="POST"
                request_data_callback={() => {
                  return {
                    command_string: `cmd/etm/enx/${ext}`,
                    send_method: "tcp_socket",
                  };
                }}
              />
            </div>
          </div>
        ))}
        <div
          style={{
            fontSize: "0.8em",
          }}
        >
          Last updated
          <Tooltip text="mm:ss:ms"> {lastUpdatedTimeStamp} ago</Tooltip>, via{" "}
          {lastUpdatedSource} - {loadingStatus}
        </div>
        <a href="/gfs/configure">Configure</a>
      </div>
      {/* <DataBlock resource="flight_data" category="extensions" /> */}
    </div>
  );
}

export default function DiagnosticsPage() {
  return (
    <>
      <PageTitle title={"Data Logs"}>@todo - add a description here</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="GFS Error Frame">
            <DataBlock resource="flight_data" category="error_frame" />
            <ErrorFrame />
          </Card>

          <Card title="Extensions Data">
            <ExtensionsData />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}
