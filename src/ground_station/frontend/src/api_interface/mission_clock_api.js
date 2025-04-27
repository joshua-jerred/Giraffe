import { useState, useEffect } from "react";
import { useApiGetData } from "./ggs_api";
import { useContext } from "react";
import { GwsGlobal } from "../GlobalContext";

const START_TIME_CHECK_INTERVAL = 2000;
const INVALID_CLOCK_TIME = "T?--:--:--";
const CLOCK_STOPPED_TIME = "T 00:00:00";

const formatTime = (
  symbol,
  days,
  hours,
  minutes,
  seconds,
  new_milliseconds = null,
  verbose = false
) => {
  let output = `T${symbol}`;
  if (days !== 0) {
    output += `${days.toString().padStart(2, "0")}:`;
  }
  output += `${hours.toString().padStart(2, "0")}:`;
  output += `${minutes.toString().padStart(2, "0")}:`;
  output += `${seconds.toString().padStart(2, "0")}`;
  if (new_milliseconds !== null && verbose) {
    output += `.${new_milliseconds.toString().padStart(3, "0")}`.slice(0, 2);
  }
  return output;
};

export const useMissionClockData = () => {
  const { showVerboseClock } = useContext(GwsGlobal);

  const [isClockValid, setIsClockValid] = useState(false);
  const [isRunning, setIsRunning] = useState(false);
  const [startTime, setStartTime] = useState(new Date());
  const [formattedTime, setFormattedTime] = useState(INVALID_CLOCK_TIME);

  // Request the mission clock data from ggs
  const { data, isDataLoading, error } = useApiGetData(
    "flight_data",
    "mission_clock",
    "all",
    START_TIME_CHECK_INTERVAL
  );

  const setClockDigits = (elapsed_time) => {
    const new_symbol = elapsed_time < 0 ? "-" : elapsed_time > 0 ? "+" : "=";

    elapsed_time = Math.abs(elapsed_time);

    const new_days = Math.floor(elapsed_time / (1000 * 60 * 60 * 24));
    const new_hours = Math.floor((elapsed_time / (1000 * 60 * 60)) % 24);
    const new_minutes = Math.floor((elapsed_time / 1000 / 60) % 60);
    const new_seconds = Math.floor((elapsed_time / 1000) % 60);
    const new_milliseconds = Math.floor(elapsed_time % 1000);

    const output = formatTime(
      new_symbol,
      new_days,
      new_hours,
      new_minutes,
      new_seconds,
      new_milliseconds,
      showVerboseClock
    );
    // console.log("output", output);

    setFormattedTime(output);
  };

  // Whenever new data is received, update the local state
  useEffect(() => {
    if (isDataLoading || error || !data) {
      return;
    }

    try {
      let input_time = data.start_time;
      let new_start_time = new Date(input_time);
      if (isNaN(new_start_time)) {
        console.log("DATA -- ", data);
        throw new Error("Invalid Date");
      }

      // Everything looks good, pass it to the local state
      setIsRunning(data.is_running);
      setIsClockValid(true);
      setStartTime(new_start_time);
    } catch (error) {
      console.error("Error parsing clock data: ", error);
      setIsClockValid(false);
    }
  }, [data, isDataLoading, error]);

  // Update the Mission Elapsed Time every 300ms
  useEffect(() => {
    if (!isClockValid) {
      // console.log("Clock is not valid");
      setFormattedTime(INVALID_CLOCK_TIME);
      return;
    }

    const updateElapsedTime = () => {
      // Calculate the elapsed time in milliseconds
      let elapsed_time = 0;
      if (isRunning) {
        elapsed_time = new Date() - startTime;
        setClockDigits(elapsed_time);
      } else {
        setClockDigits(0);
      }
    };

    // run it once right away, then set the interval
    updateElapsedTime();
    const interval = setInterval(
      () => {
        updateElapsedTime();
      },
      showVerboseClock ? 50 : 100
    );
    return () => clearInterval(interval);

    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [startTime, isClockValid, isRunning, showVerboseClock]);

  return {
    isClockValid,
    isClockRunning: isRunning,
    formattedTime,
  };
};
