import { useState, useEffect, useContext } from "react";
import { StyButton, SelectMenu } from "./styled/StyledComponents";
import { useTheme } from "styled-components";
import styled from "styled-components";
import { GwsGlobal } from "../GlobalContext";

const RequestButton = styled(StyButton)`
  white-space: nowrap;
  flex-grow: 1;
  padding: 0.2rem;
  max-width: 30em;
  background: ${(props) =>
    props.enabled ? props.theme.primary : props.theme.primary_soft};

  &:hover {
    background: ${(props) =>
      props.enabled ? props.theme.primary_hover : props.theme.primary_soft};
    cursor: ${(props) => (props.enabled ? "pointer" : "default")};
  }

  &:active {
    filter: ${(props) => (props.enabled ? "brightness(0.9)" : "none")};
  }
`;

/**
 * @param {bool} button_loading_override - Set to true to keep the button
 * disabled and in a "loading" state.
 * @param {string} unsafe - Set to true if the API request is considered unsafe.
 * This will then require unsafe mode to be enabled.
 */
export function ApiRequestButton({
  api_endpoint,
  title = "submit",
  request_data_callback = () => {},
  success_callback = () => {},
  error_callback = () => {},
  button_loading_override = null,
  style = {},
  api_method = "PUT",
  confirmation_message = null,
  unsafe = false,
}) {
  // idle, loading, success, error
  const [requestState, setRequestState] = useState("idle");
  const [buttonContent, setButtonContent] = useState(title);
  const [buttonEnabled, setButtonEnabled] = useState(true);
  const [error, setError] = useState(null);
  const { isGgsConnected, ggsAddress, unsafeMode } = useContext(GwsGlobal);

  // Used to briefly show the success or error state
  const temporaryState = (new_temporary_state) => {
    setRequestState(new_temporary_state);

    setTimeout(() => {
      setRequestState("idle");
    }, 2000);
  };

  const sendRequest = (data) => {
    if (!isGgsConnected) {
      setError("GGS not connected");
      setRequestState("error");
      return;
    }

    if (confirmation_message !== null) {
      if (!window.confirm(confirmation_message)) {
        return;
      }
    } else if (unsafe) {
      // unsafe but no confirmation message
      if (
        !window.confirm(
          "This action requires unsafe mode. Are you sure you want to do this?"
        )
      ) {
        return;
      }
    }

    setRequestState("loading");
    const url = `${ggsAddress}${api_endpoint}`;
    fetch(url, {
      method: api_method,
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(data),
    })
      .then((response) => {
        // const json_data = response.json();
        if (!response.ok) {
          throw new Error("response code was not ok " + response.status);
        } else {
          temporaryState("success");
          success_callback(true);
        }
      })
      .catch((error) => {
        setError(error.what);
        temporaryState("error");
      });
  };

  useEffect(() => {
    if (unsafe && !unsafeMode) {
      setButtonEnabled(false);
      setError("unsafe mode required");
      setRequestState("unsafe");
      return;
    }

    if (requestState === "idle") {
      setButtonContent(title);
      setButtonEnabled(
        // Don't enable the button if the loading override is set to true
        button_loading_override === null ? true : !button_loading_override
      );
    } else if (requestState === "loading") {
      setButtonContent("loading");
      setButtonEnabled(false);
    } else if (requestState === "success") {
      setButtonContent("success");
      setButtonEnabled(false);
    } else if (requestState === "error") {
      setButtonContent("error");
      setButtonEnabled(false);
    } else {
      setButtonContent("unknown state");
      setButtonEnabled(false);
    }
  }, [requestState, button_loading_override, title]);

  return (
    <RequestButton
      onClick={() => {
        if (requestState === "idle") {
          sendRequest(request_data_callback());
          return;
        }
      }}
      enabled={buttonEnabled}
      style={style}
    >
      {buttonContent}{" "}
      {requestState === "loading" || button_loading_override ? (
        <i className="fa fa-spinner fa-spin"></i>
      ) : null}
      {requestState === "success" ? <i className="fa fa-check"></i> : null}
      {error && (
        <p
          style={{
            textAlign: "center",
            margin: "auto",
            paddingLeft: "0.5rem",
            paddingRight: "0.5rem",
            borderStyle: "solid",
            boarderRadius: "1rem",
            width: "fit-content",
          }}
        >
          {error}
        </p>
      )}
    </RequestButton>
  );
}

export function ApiRequestDropdown({
  api_endpoint,
  options,
  data_key = "key",
  title = "submit",
  unsafe = false,
  no_loading = false,
}) {
  const { ggsAddress } = useContext(GwsGlobal);

  const [selected, setSelected] = useState(options[0]);
  const [loadedInitialData, setLoadedInitialData] = useState(no_loading);

  const requestDataCallback = () => {
    return { [data_key]: selected };
  };

  const successCallback = () => {
    console.log("success");
  };

  const errorCallback = () => {
    console.log("error");
  };

  useEffect(() => {
    if (loadedInitialData) {
      return;
    }

    const url = `${ggsAddress}${api_endpoint}`;
    fetch(url)
      .then((response) => response.json())
      .then((data) => {
        setSelected(data[data_key]);
        setLoadedInitialData(true);
      })
      .catch((error) => {
        console.error("Error:", error);
      });
  }, [
    setLoadedInitialData,
    ggsAddress,
    api_endpoint,
    loadedInitialData,
    data_key,
  ]);

  return (
    <div
      style={{
        display: "flex",
        alignItems: "flex-end",
        flexWrap: "wrap",
        gap: "10px",
        marginTop: "1rem",
        marginBottom: "1rem",
        justifyContent: "center",
      }}
    >
      <SelectMenu
        options={options}
        selected={selected}
        setSelected={setSelected}
        style={{ flexGrow: 1, padding: "0.4rem", maxWidth: "70%" }}
      />
      <ApiRequestButton
        api_endpoint={api_endpoint}
        title={title}
        request_data_callback={requestDataCallback}
        success_callback={successCallback}
        error_callback={errorCallback}
        style={{ flexGrow: 1, maxWidth: "30%" }}
        button_loading_override={loadedInitialData === false}
        unsafe={unsafe}
      />
    </div>
  );
}
