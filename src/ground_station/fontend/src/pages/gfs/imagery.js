import { useState, useEffect, useContext } from "react";

import { GwsGlobal } from "../../GlobalContext";

import styled from "styled-components";

import {
  PageTitle,
  PageContent,
  Card,
  CardMasonryLayout,
  CardBreak,
  CardSectionTitle,
} from "../../core/PageParts";
import { StyButton } from "../../components/styled/StyledComponents";

const ImageListingContainer = styled.div`
  > span {
    margin: 1em;
  }
  > .title {
    font-weight: bold;
  }
  > ${StyButton} {
    margin: 1em;
  }

  > .download-button {
    display: block;
    margin: auto;
    margin-top: 1em;
  }
`;

const ImageContainer = styled.img`
  max-width: 100%;
  max-height: 400px;
  display: ${(props) => (props.open ? "block" : "none")};
  margin: auto;
`;

function ImageListing({ name, data }) {
  const { ggsAddress, isGfsTcpConnected } = useContext(GwsGlobal);
  const [imageOpen, setImageOpen] = useState(false);
  const [onLocal, setOnLocal] = useState(data.local);
  const [onRemote, setOnRemote] = useState(data.remote);

  const [isSyncing, setIsSyncing] = useState(false);
  const [syncProgress, setSyncProgress] = useState("");
  const [syncError, setSyncError] = useState(null);

  useEffect(() => {
    if (syncError) {
      setTimeout(() => {
        setSyncError(null);
      }, 10000);
    }
  }, [syncError]);

  useEffect(() => {
    if (isSyncing) {
      const interval = setInterval(() => {
        setSyncProgress((prev) => {
          if (prev.length > 3) {
            return "";
          }
          return prev + ".";
        });
      }, 250);
      return () => clearInterval(interval);
    }
  }, [isSyncing]);

  const syncImage = () => {
    setSyncProgress("");
    setIsSyncing(true);
    fetch(`${ggsAddress}/api/gfs/images/sync?filename=${name}`, {
      method: "PUT",
      timeout: 10000,
    })
      .then((response) => {
        if (!response.ok) {
          setSyncError("Non-OK response");
          setIsSyncing(false);
          return;
        }

        setOnLocal(true);
        setSyncError(null);
        setIsSyncing(false);
      })
      .catch((error) => {
        console.error("Error syncing GFS imagery", error);
        setSyncError(error);
        setIsSyncing(false);
      });
  };

  return (
    <ImageListingContainer>
      <span className="title">{`${name}`}</span>
      <span>
        {onLocal ? "local" : ""}
        {onLocal && onRemote ? " & " : ""}
        {onRemote ? "remote" : ""}
      </span>
      {onLocal && (
        <StyButton onClick={() => setImageOpen(!imageOpen)}>
          {imageOpen ? "Close" : "Open"}
        </StyButton>
      )}
      {onRemote && isGfsTcpConnected && (
        <StyButton onClick={() => syncImage()}>Sync</StyButton>
      )}
      {syncError !== null && (
        <span style={{ color: "red" }}>Error syncing: {syncError}</span>
      )}
      {isSyncing && <span>Syncing{syncProgress}</span>}
      {onLocal && (
        <>
          <ImageContainer
            src={`${ggsAddress}/ggs_images/${name}`}
            open={imageOpen}
            alt={`${ggsAddress}/ggs_images/${name}`}
          />
          {imageOpen && (
            <StyButton
              className="download-button"
              onClick={() => {
                window.open(`${ggsAddress}/ggs_images/${name}`, "_blank");
              }}
            >
              Download
            </StyButton>
          )}
        </>
      )}
    </ImageListingContainer>
  );
}

function GfsImageryPage() {
  const { ggsAddress, isGgsConnected } = useContext(GwsGlobal);

  const [imagesData, setImagesData] = useState({});
  const [error, setError] = useState(null);

  useEffect(() => {
    if (!isGgsConnected) {
      return;
    }

    fetch(`${ggsAddress}/api/gfs/images`)
      .then((response) => {
        if (!response.ok) {
          throw new Error("Non-OK response");
        }
        return response.json();
      })
      .then((data) => {
        setImagesData(data);
      })
      .catch((error) => {
        console.error("Error fetching GFS imagery data", error);
        setError(error);
      });
  }, [ggsAddress, isGgsConnected]);

  return (
    <>
      <PageTitle title="GFS Imagery">`/// @todo Add description`</PageTitle>
      <PageContent>
        {/* <CardMasonryLayout> */}
        <Card title="Images">
          <CardSectionTitle>Imagery Stats</CardSectionTitle>
          <p>todo</p>
          {error && <div>Error: {error.message}</div>}
          {Object.keys(imagesData).map((key) => {
            return (
              <>
                <CardBreak />
                <ImageListing key={key} name={key} data={imagesData[key]} />
              </>
            );
          })}
        </Card>
        {/* </CardMasonryLayout> */}
      </PageContent>
    </>
  );
}

export default GfsImageryPage;
