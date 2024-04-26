import { useState, useEffect } from "react";

import { CardSectionTitle, CardContentCentered } from "../core/PageParts";
import { useApiGetData } from "../api_interface/ggs_api";

const START_TIME_CHECK_INTERVAL = 10000;

export function MissionClock() {
  const [startTime, setStartTime] = useState(new Date());
  const [isClockValid, setIsClockValid] = useState(true);
  const [isClockLocked, setIsClockLocked] = useState(true);

  const [days, setDays] = useState(0);
  const [hours, setHours] = useState(0);
  const [minutes, setMinutes] = useState(0);
  const [seconds, setSeconds] = useState(0);

  const { data, isDataLoading, error } = useApiGetData(
    "flight_data",
    "mission_clock",
    "all",
    START_TIME_CHECK_INTERVAL
  );

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
      setStartTime(new_start_time);
      setIsClockLocked(data.start_time_locked);
      setIsClockValid(true);
    } catch (error) {
      console.error("Error parsing date", error);
      setIsClockValid(false);
    }
  }, [data, isDataLoading, error]);

  useEffect(() => {
    if (!isClockValid) {
      return;
    }

    const interval = setInterval(() => {
      let ellapsed_time = new Date() - startTime;
      setDays(Math.floor(ellapsed_time / (1000 * 60 * 60 * 24)));
      setHours(Math.floor((ellapsed_time / (1000 * 60 * 60)) % 24));
      setMinutes(Math.floor((ellapsed_time / 1000 / 60) % 60));
      setSeconds(Math.floor((ellapsed_time / 1000) % 60));
    }, 1000);
    return () => clearInterval(interval);
  }, [startTime, isClockValid]);

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
                <h2>
                  {days}d {hours}h {minutes}m {seconds}s
                </h2>
                <p>Start Time: {startTime.toUTCString()}</p>
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
