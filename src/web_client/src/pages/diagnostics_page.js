import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
} from "../core/PageParts";
import { useApiGetData } from "../api_interface/ggs_api";
import { DataBlock } from "../components/DataBlock";
import { useEffect, useState } from "react";

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

    const NEEDED_PROPERTIES = ["last_updated_timestamp", "last_updated_source"];
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
    setLastUpdatedTimeStamp(extensionsData.values.last_updated_timestamp);
    setLastUpdatedSource(extensionsData.values.last_updated_source);
  }, [extensionsData, extensionsDataError, isExtensionsDataLoading]);

  return (
    <div>
      <div
        style={{
          fontSize: "0.8em",
        }}
      >
        Last Updated {lastUpdatedTimeStamp} seconds ago, via {lastUpdatedSource}{" "}
        - {loadingStatus}
      </div>

      <a href="/gfs/configure">Configure</a>
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
