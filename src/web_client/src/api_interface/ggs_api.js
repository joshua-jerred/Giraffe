import { useState, useContext, useEffect } from "react";
import { GwsGlobal } from "../GlobalContext";

export const useApiGetData = (
  resource,
  category,
  include = "all",
  update_interval = -1
) => {
  const { ggsAddress, isGgsConnected } = useContext(GwsGlobal);
  const [data, setData] = useState(null);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState(null);

  useEffect(() => {
    const fetchData = async () => {
      setIsLoading(true);
      // if (!isGgsConnected) {
      // setError("Not connected to GGS");
      // return;
      // }
      try {
        const encoded_category = encodeURIComponent(category);
        const path = `${ggsAddress}/api/${resource}/data?category=${encoded_category}&include=${include}`;
        const response = await fetch(path);
        if (!response.ok) {
          throw new Error("Failed to load metadata.");
        }
        const data = await response.json();
        setData(data);
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
  ]);

  return { data, isLoading, error };
};

export const UseGenericGetApi = (api_endpoint, update_interval = -1) => {
  const [data, setData] = useState(null);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState(null);
  const { ggsAddress, isGgsConnected } = useContext(GwsGlobal);

  const path = `${ggsAddress}${api_endpoint}`;

  useEffect(() => {
    const fetchData = async () => {
      setIsLoading(true);
      try {
        const response = await fetch(path);
        if (!response.ok) {
          throw new Error("Failed to load metadata.");
        }
        const data = await response.json();
        setData(data);
        setError(null);
        setIsLoading(false);
        console.log(data);
      } catch (error) {
        setError(error.message);
        setIsLoading(false);
      }
    };

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

  return { data, isLoading, error };
};