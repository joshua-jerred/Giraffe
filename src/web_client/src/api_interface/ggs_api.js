import { useState, useContext, useEffect } from "react";
import { GwsGlobal } from "../GlobalContext";

export const useApiGetData = (
  resource,
  category,
  include = "all",
  update_interval = -1
) => {
  const [data, setData] = useState(null);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState(null);
  const { ggsAddress, isGgsConnected } = useContext(GwsGlobal);

  const valid_include = ["all", "values", "metadata"];
  if (!valid_include.includes(include)) {
    throw new Error(`Invalid include: ${include}`);
  }

  const valid_resource = ["ggs", "gfs", "gdl", "flight_data"];
  if (!valid_resource.includes(resource)) {
    throw new Error(`Invalid resource: ${resource}`);
  }

  const encoded_category = encodeURIComponent(category);
  const path = `${ggsAddress}/api/${resource}/data?category=${encoded_category}&include=${include}`;

  const fetchData = async () => {
    setIsLoading(true);
    if (!isGgsConnected) {
      setError("Not connected to GGS");
      return;
    }
    try {
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

  useEffect(() => {
    // if (isGgsConnected) {
    fetchData();
    // } else {
    //   setError("Not connected to GGS");
    // }

    if (update_interval > 0) {
      const interval = setInterval(() => {
        fetchData();
      }, update_interval);
      return () => clearInterval(interval);
    }
  }, [isGgsConnected]);

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
