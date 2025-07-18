import { useState, useContext, useEffect } from "react";
import { GwsGlobal } from "../GlobalContext";

// Use this one if metadata exists (it's an older API path)
export const useApiGetData = (
  resource, // flight_data, gfs, gdl, fsa, command
  category, // subcategory of the resource
  include = "all", // values, metadata, all
  update_interval = -1, // optional update interval in ms, -1 means no update
  full_api_path = null // optional full API path to override the default one
) => {
  const { ggsAddress, isGgsConnected } = useContext(GwsGlobal);
  const [data, setData] = useState(null);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState(null);
  const [needUpdate, setNeedUpdate] = useState(false);

  useEffect(() => {
    const fetchData = async () => {
      setIsLoading(true);
      // if (!isGgsConnected) {
      // setError("Not connected to GGS");
      // return;
      // }
      try {
        const encoded_category = encodeURIComponent(category);
        let path = "";

        if (full_api_path) {
          path = `${ggsAddress}${full_api_path}`;
        } else {
          path = `${ggsAddress}/api/${resource}/data?category=${encoded_category}&include=${include}`;
        }
        const response = await fetch(path);
        if (!response.ok) {
          throw new Error("Failed to load metadata.");
        }
        const data = await response.json();
        setData(data);
        setNeedUpdate(false);
        // console.log("updated");
      } catch (error) {
        setError(error.message);
      }
      setIsLoading(false);
    };

    // if (isGgsConnected) {
    fetchData();
    // } else {
    // setError("Not connected to GGS");
    // }

    if (update_interval > 0) {
      const interval = setInterval(() => {
        fetchData();
      }, update_interval);
      return () => clearInterval(interval);
    }
  }, [
    isGgsConnected,
    update_interval,
    category,
    resource,
    ggsAddress,
    include,
    needUpdate,
    full_api_path,
  ]);

  return { data, isLoading, error, setNeedUpdate };
};

// This appears to be the slightly more generic version of what is above.
// Use the other one if metadata is defined.
export const useGenericGetApi = (api_endpoint, update_interval = -1) => {
  const [data, setData] = useState(null);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState(null);
  const { ggsAddress, isGgsConnected } = useContext(GwsGlobal);

  const path = `${ggsAddress}${api_endpoint}`;

  const fetchData = async () => {
    setIsLoading(true);
    try {
      const response = await fetch(path);
      if (!response.ok) {
        throw new Error(`Failed to load data from ${path}`);
      }
      const data = await response.json();
      setData(data);
      setError(null);
      setIsLoading(false);
      // console.log(data);
    } catch (error) {
      setError(error.message);
      setIsLoading(false);
    }
  };

  const forceUpdate = () => {
    fetchData();
  };

  useEffect(() => {
    if (isGgsConnected) {
      fetchData();
    } else {
      setError("Not connected to GGS");
    }

    if (update_interval > 0) {
      const interval = setInterval(() => {
        fetchData();
      }, update_interval);
      return () => clearInterval(interval);
    }
  }, [isGgsConnected, update_interval, path]);

  return { data, isLoading, error, forceUpdate };
};
