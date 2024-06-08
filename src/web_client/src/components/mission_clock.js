import { useState, useEffect, useContext } from "react";

import {
  StyWarningButton,
  StyInput,
} from "../components/styled/StyledComponents";
import { CardSectionTitle, CardContentCentered } from "../core/PageParts";
import { useApiGetData } from "../api_interface/ggs_api";
import { GwsGlobal } from "../GlobalContext";

const START_TIME_CHECK_INTERVAL = 2000;

function MissionClockControlMenu({
  isRunning,
  skew,
  setSkew,
  clockUpdateCallback,
}) {
  const { ggsAddress } = useContext(GwsGlobal);

  const startMissionClock = (skew) => {
    // if (!window.confirm("Are you sure you want to start the mission clock?")) {
    // return;
    // }

    fetch(`${ggsAddress}/api/flight_data/data?category=start_mission_clock`, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({ clock_skew: skew }),
    })
      .then((response) => response.json())
      .then((data) => {
        if (data.status === "success") {
          clockUpdateCallback();
        } else {
          console.error("Error starting mission clock", data);
        }
      })
      .catch((error) => {
        console.error("Error:", error);
      });
  };

  const stopMissionClock = () => {
    fetch(`${ggsAddress}/api/flight_data/data?category=stop_mission_clock`, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json",
      },
    })
      .then((response) => response.json())
      .then((data) => {
        if (data.status === "success") {
          clockUpdateCallback();
        } else {
          console.error("Error stopping mission clock", data);
        }
      })
      .catch((error) => {
        console.error("Error:", error);
      });
  };

  return (
    <CardContentCentered>
      {isRunning === true ? (
        <>
          <StyWarningButton
            onClick={() => {
              stopMissionClock();
            }}
          >
            Stop
          </StyWarningButton>
        </>
      ) : (
        <div
          style={{
            display: "flex",
            flexDirection: "row",
            alignItems: "center",
            marginTop: "1em",
          }}
        >
          <StyInput
            type="number"
            placeholder="Skew Time (s)"
            value={skew}
            onChange={(e) => {
              if (e.target.value < 0) {
                setSkew(0);
              } else if (isNaN(e.target.value)) {
                setSkew(0);
              } else {
                setSkew(e.target.value);
              }
            }}
            style={{
              textAlign: "center",
              height: "2em",
            }}
          />
          <StyWarningButton
            style={{
              height: "2em",
            }}
            onClick={() => {
              startMissionClock(skew);
            }}
          >
            Start Clock
          </StyWarningButton>
        </div>
      )}

      <p
        style={{
          marginTop: "2em",
          textAlign: "center",
        }}
      >
        {isRunning === true
          ? ""
          : "Start the Mission Clock with an optional clock skew. Depending on sequencing setup a skew will be required."}
      </p>
    </CardContentCentered>
  );
}

function MissionTimer({ startTime }) {}

function ClockMartix({ ggsTime, gfsTime, gfsTimeSkew, gfsGpsTime }) {
  const [localTime, setLocalTime] = useState("--:--:--");

  useEffect(() => {
    const setLocalTimeDisplay = () => {
      let current_time = new Date();
      let hours = current_time.getUTCHours();
      if (hours < 10) {
        hours = "0" + hours;
      }
      let minutes = current_time.getUTCMinutes();
      if (minutes < 10) {
        minutes = "0" + minutes;
      }
      let seconds = current_time.getUTCSeconds();
      if (seconds < 10) {
        seconds = "0" + seconds;
      }
      setLocalTime(`${hours}:${minutes}:${seconds}`);
    };

    setLocalTimeDisplay();

    const interval = setInterval(() => {
      setLocalTimeDisplay();
    }, 1000);
    return () => clearInterval(interval);
  }, []);

  const clock_style = {
    margin: "0px",
    fontSize: "1em",
    fontWeight: "normal",
    display: "flex",
    flexDirection: "row",
    justifyContent: "space-between",
  };

  return (
    <div style={{ width: "70%", paddingTop: "10px", paddingBottom: "10px" }}>
      {/* <CardContentCentered> */}
      <div style={clock_style}>
        <span>Local Time</span>
        <span>{localTime} UTC</span>
      </div>
      <div style={clock_style}>
        <span>GGS</span>
        <span>{ggsTime} UTC</span>
      </div>
      <div style={clock_style}>
        <span>GFS</span>
        <span>{gfsTime} UTC</span>
      </div>
      <div style={clock_style}>
        <span>GFS Skew</span>
        <span>{gfsTimeSkew}s</span>
      </div>
      <div style={clock_style}>
        <span>GFS GPS</span>
        <span>{gfsGpsTime} UTC</span>
      </div>
      {/* </CardContentCentered> */}
    </div>
  );
}

export function MissionClock() {
  const [isClockMenuOpen, setClockMenuOpen] = useState(false);
  const [startSkewTime, setStartSkewTime] = useState(0);
  const [isRunning, setIsRunning] = useState(true);
  const [startTime, setStartTime] = useState(new Date());
  const [isClockValid, setIsClockValid] = useState(true);
  const [clockSignSymbol, setClockSignSymbol] = useState("+");

  const [days, setDays] = useState(0);
  const [hours, setHours] = useState(0);
  const [minutes, setMinutes] = useState(0);
  const [seconds, setSeconds] = useState(0);

  const [ggsTime, setGgsTime] = useState("--:--:--");
  const [gfsTime, setGfsTime] = useState("--:--:--");
  const [gfsTimeSkew, setGfsTimeSkew] = useState(0);
  const [gfsGpsTime, setGfsGpsTime] = useState("--:--:--");

  const { data, isDataLoading, error, setNeedUpdate } = useApiGetData(
    "flight_data",
    "mission_clock",
    "all",
    START_TIME_CHECK_INTERVAL
  );

  const setClockDigits = (elapsed_time) => {
    if (elapsed_time < 0) {
      setClockSignSymbol("-");
      elapsed_time = Math.abs(elapsed_time);
    }

    setDays(Math.floor(elapsed_time / (1000 * 60 * 60 * 24)));
    setHours(Math.floor((elapsed_time / (1000 * 60 * 60)) % 24));
    setMinutes(Math.floor((elapsed_time / 1000 / 60) % 60));
    setSeconds(Math.floor((elapsed_time / 1000) % 60));
  };

  useEffect(() => {
    if (isDataLoading || error || !data) {
      return;
    }

    try {
      let input_time = data.start_time;
      let new_start_time = new Date(input_time);
      if (isNaN(new_start_time)) {
        throw new Error("Invalid Date");
      }
      setIsRunning(data.is_running);
      setIsClockValid(true);

      setGgsTime(data.ggs_utc_time);
      setGfsTime(data.gfs_utc_time);
      setGfsTimeSkew(data.gfs_time_skew);
      setGfsGpsTime(data.gfs_gps_utc_time);

      setStartTime(new_start_time);
    } catch (error) {
      console.error("Error parsing clock data: ", error);
      setIsClockValid(false);
    }
  }, [data, isDataLoading, error]);

  useEffect(() => {
    if (!isClockValid) {
      return;
    }

    const updateElapsedTime = () => {
      // Calculate the elapsed time in milliseconds
      let elapsed_time = 0;
      if (isRunning) {
        elapsed_time = new Date() - startTime;
        setClockDigits(elapsed_time);
      }
    };

    updateElapsedTime();

    const interval = setInterval(() => {
      updateElapsedTime();
    }, 300);
    return () => clearInterval(interval);

    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [startTime, isClockValid, isRunning]);

  useEffect(() => {
    if (!isRunning) {
      let skew_time = parseInt(startSkewTime);
      setClockDigits(skew_time * -1000);
    }
  }, [startSkewTime, isRunning]);

  return (
    <div>
      <CardSectionTitle>Mission Clock</CardSectionTitle>
      {isDataLoading ? (
        <p>Loading...</p>
      ) : error ? (
        <p>Error: {error}</p>
      ) : (
        <div>
          {isClockValid ? (
            <div>
              <CardContentCentered>
                <h2
                  onClick={() => {
                    setClockMenuOpen(!isClockMenuOpen);
                  }}
                  className="noselect"
                  style={{ cursor: "pointer", margin: "0px" }}
                >
                  {clockSignSymbol} {days}d {hours}h {minutes}m {seconds}s
                </h2>
                {isRunning ? (
                  <p>Start Time: {startTime.toUTCString()}</p>
                ) : (
                  <h4 style={{ margin: "0px" }}>Stopped</h4>
                )}
                <ClockMartix
                  ggsTime={ggsTime}
                  gfsTime={gfsTime}
                  gfsTimeSkew={gfsTimeSkew}
                  gfsGpsTime={gfsGpsTime}
                />
                {isClockMenuOpen && (
                  <MissionClockControlMenu
                    isRunning={isRunning}
                    skew={startSkewTime}
                    setSkew={setStartSkewTime}
                    clockUpdateCallback={() => {
                      setNeedUpdate(true);
                      setClockMenuOpen(false);
                    }}
                  />
                )}
              </CardContentCentered>
            </div>
          ) : (
            <p>Invalid Start Time</p>
          )}
        </div>
      )}
    </div>
  );
}
