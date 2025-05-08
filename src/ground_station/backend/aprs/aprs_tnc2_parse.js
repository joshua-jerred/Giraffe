const log = require("../helpers/logger").createLogger("aprs-parser");

function base91Decode(data) {
  let value = 0;
  for (let i = 0; i < data.length; i++) {
    value += (data.charCodeAt(i) - 33) * Math.pow(91, data.length - i - 1);
  }
  return value;
}

function parseTimestamp(data) {
  const pattern = /([0-9]{2})([0-9]{2})([0-9]{2})([zh\/])/;
  const match = data.match(pattern);
  if (!match) {
    log.warning("Invalid timestamp data:", data);
    return null;
  }

  let timestamp_data = {
    raw: match[0],
    timezone: null,
    format: null,
    day: null,
    hour: null,
    minute: null,
    second: null,
  };

  let type_char = match[4];
  if (type_char === "z" || type_char === "/") {
    timestamp_data.timezone = type_char === "z" ? "zulu" : "local";
    timestamp_data.format = "ddhhmm";
    timestamp_data.day = parseInt(match[1], 10);
    timestamp_data.hour = parseInt(match[2], 10);
    timestamp_data.minute = parseInt(match[3], 10);
    timestamp_data.second = 0;
  } else if (type_char === "h") {
    timestamp_data.timezone = "zulu";
    timestamp_data.format = "hhmmss";
    timestamp_data.hour = parseInt(match[1], 10);
    timestamp_data.minute = parseInt(match[2], 10);
    timestamp_data.second = parseInt(match[3], 10);
  } else {
    log.warning("Invalid timestamp type:", type_char);
    return null;
  }

  return timestamp_data;
}

function parseWeatherData(data) {
  let weather_data = {
    wind_direction: null,
    wind_speed_mph: null,
    gust_speed_mph: null,
    temperature_f: null,
    rainfall_1h: null,
    rainfall_24h: null,
    rainfall_since_midnight: null,
    humidity: null,
    barometric_pressure: null,
  };

  // first, wind speed and direction as these can be encoded in two ways
  const wind_dir_speed_pattern =
    /(?:(?:([\ \.0-9]{3})\/([\ 0-9]{3}))(?![0-9])|(?:c[\ \.]*([0-9]{1,3})s[\ \.]*([0-9]{0,3})))(.*)/;
  const wind_dir_speed_match = data.match(wind_dir_speed_pattern);
  // match[1] | match[3] = wind direction
  // match[2] | match[4] = wind speed
  // match[5] = rest of the data (this is how we know if it's a weather report)

  if (
    wind_dir_speed_match === null ||
    typeof wind_dir_speed_match[5] !== "string"
  ) {
    return null;
  }
  if (wind_dir_speed_match) {
    if (wind_dir_speed_match[1]) {
      weather_data.wind_direction = parseInt(wind_dir_speed_match[1], 10);
      weather_data.wind_speed_mph = parseInt(wind_dir_speed_match[2], 10);
    } else if (wind_dir_speed_match[3]) {
      weather_data.wind_direction = parseInt(wind_dir_speed_match[3], 10);
      weather_data.wind_speed_mph = parseInt(wind_dir_speed_match[4], 10);
    }
  }

  const pattern = /([gtrhbpPhLl#])([0-9\.\ ]{2,5})/g;
  const match = wind_dir_speed_match[5].matchAll(pattern);
  if (!match) {
    return null;
  }

  for (const m of match) {
    const key = m[1];
    const value = parseFloat(m[2].replace(/[\s\.\ \-]/g, ""));
    switch (key) {
      case "g":
        weather_data.gust_speed_mph = value;
        break;
      case "t":
        weather_data.temperature_f = value;
        break;
      case "r":
        weather_data.rainfall_1h = value;
        break;
      case "p":
        weather_data.rainfall_24h = value;
        break;
      case "P":
        weather_data.rainfall_since_midnight = value;
        break;
      case "h":
        weather_data.humidity = value;
        break;
      case "b":
        weather_data.barometric_pressure = value / 10.0; // convert to millibars
        break;
      case "L":
        weather_data["luminosity"] = value;
        break;
      case "l":
        weather_data["luminosity_high"] = value;
        break;
      case "s":
        weather_data["snow_fall_24h"] = value;
        break;
      case "#":
        weather_data["raw_rain_counter"] = value;
        break;
      default:
        log.warning("Unknown weather data type:", m[1]);
    }
  }
  // console.log("--");

  return {
    ...weather_data,
    match: match,
  };
}

function hasWeatherData(data) {
  return parseWeatherData(data) !== null;
}

function parseCompressedPosition(data) {
  const pattern = /([\\\/])(.{4})(.{4})(.)(.)(.)(.)/;
  const match = data.match(pattern);
  if (!match) {
    log.warning("Invalid compressed position data:", data);
    return null;
  }

  let lat = 90 - base91Decode(match[2]) / 380926;
  let lon = -180 + base91Decode(match[3]) / 190463;

  let course = null;
  let speed = null;
  let range = null;
  let altitude_m = null; // in meters

  // csT bytes
  let c_byte = match[5];
  let c_val = c_byte.charCodeAt(0);
  let s_byte = match[6];
  let s_val = s_byte.charCodeAt(0);
  let T_val = match[7].charCodeAt(0);
  console.log("csT data:", c_byte, s_byte, T_val);
  if (c_byte === " ") {
    // if c is space, csT is ignored
    console.log("no csT data");
  } else if (c_byte === "{") {
    range = 2 * Math.pow(1.08, s_val);
  } else if ((T_val >> 4) & 1 && !((T_val >> 3) & 1)) {
    // GGA Source indicated by bits 4 and 3 being `10`
    console.log("altitude data in csT");
    let cs = (c_val - 33) * 91 + (s_val - 33);
    altitude_m = Math.pow(1.002, cs);
    altitude_m = altitude_m * 0.3048; // convert to meters
  } else if (c_val >= 0 && c_val <= 89) {
    // '!' < c < 'Z'
    course = c_val * 4;
    speed = Math.pow(1.08, s_val) - 1;
  } else {
    console.log("Invalid csT data");
  }

  let compressed_data = {
    latitude: lat,
    longitude: lon,
    symbol_table_id: match[1],
    symbol_code: match[4],
    course: course,
    speed: speed,
    range: range,
    altitude: altitude_m, // in meters
    compressed: true,
    compression_byte: T_val,
  };

  return compressed_data;
}

function parsePositionDataExtension(data) {
  // console.log("parsePositionDataExtension data:", data);

  let data_extension_start = 0;
  let return_data = {
    data_extension_type: "none",
    data_extension: {
      // brg: null,
      // nrq: null,
      // phg: null,
      // range: null,
      // altitude: null,
    },
  };

  const course_speed_brg_nrq_pattern =
    /([0-9]{3})\/([0-9]{3})(?:\/([0-9]{3})\/([0-9]{3}))?/;
  const power_efficiency_pattern = /PHG(.{4})/;
  const range_pattern = /RNG([0-9]{4})/;
  const df_signal_strength_pattern = /DFS([0-9]{4})/;
  const area_opject_discriptor_pattern = /T(.{2})\/C(.{2})/;

  let match = data.match(course_speed_brg_nrq_pattern);
  if (match && !hasWeatherData(data)) {
    // If there is weather data, these values are actually the wind direction and speed
    return_data.course = parseInt(match[1], 10);
    return_data.speed = parseInt(match[2], 10);
    data_extension_start += 7;

    let brg = match[3] ? parseInt(match[3], 10) : null;
    let nrq = match[4] ? parseInt(match[4], 10) : null;

    if (brg && nrq) {
      data_extension_start += 7;
      return_data.data_extension_type = "course-speed-brg-nrq";
      return_data.data_extension.brg = brg;
      return_data.data_extension.nrq = nrq;
    } else {
      return_data.data_extension_type = "course-speed";
    }
  }

  match = data.match(power_efficiency_pattern);
  if (match) {
    // console.log("power_efficiency data:", match[0]);
    return_data.data_extension.phg = match[1];
    return_data.data_extension_type = "power-height-gain";
    data_extension_start = 7;
  }

  match = data.match(range_pattern);
  if (match) {
    console.log("range data:", match[0]);
    return_data.data_extension.range = parseInt(match[1], 10);
    return_data.data_extension_type = "range";
    data_extension_start = 7;
  }

  match = data.match(df_signal_strength_pattern);
  if (match) {
    console.log("df_signal_strength data:", match[0]);
    return_data.data_extension.dfs = parseInt(match[1], 10);
    return_data.data_extension_type = "df-signal-strength";
    data_extension_start = 7;
  }

  match = data.match(area_opject_discriptor_pattern);
  if (match) {
    console.log("area_opject_discriptor data:", match[0]);
    return_data.data_extension.t = match[1];
    return_data.data_extension.c = match[2];
    return_data.data_extension_type = "area-object-descriptor";
    data_extension_start = 7;
  }

  // The altitude is in the comment, not in the data extension
  return_data.comment = data.substring(data_extension_start);
  const altitude_comment_pattern = /\/A\=([0-9]{0,8})/;
  match = data.match(altitude_comment_pattern);
  if (match) {
    let altitude = parseInt(match[1], 10);
    if (altitude >= 0 && altitude <= 99999) {
      return_data.altitude = altitude;
    }
  }

  if (return_data.data_extension_type === "none") {
    return_data.data_extension = null;
  }

  return return_data;
}

function parsePosition(data) {
  if (data.at(0) === "/" || data.at(0) === "\\") {
    return parseCompressedPosition(data);
  }

  const pattern =
    /([0-9\ ]{2})([0-9\ ]{2})\.([0-9\ ]{2})([NS])(.)([0-9\ ]{3})([0-9\ ]{2})\.([0-9\ ]{2})([WE])(.)/;
  const match = data.match(pattern);

  if (!match) {
    log.warning("Invalid position data:", data);
    return null;
  }

  const ddMmSsToDecimal = (degrees, minutes, seconds) => {
    const toNumber = (value) => {
      let new_value = value.replace(/\s/g, "");
      let number = parseFloat(new_value, 10);
      if (isNaN(number)) {
        return 0;
      }
      return number;
    };

    let value = 0;
    value += toNumber(degrees);
    value += toNumber(minutes) / 60;
    value += toNumber(seconds) / 3600;
    return value;
  };

  let coordinate_data = {
    latitude:
      ddMmSsToDecimal(match[1], match[2], match[3]) *
      (match[4] === "N" ? 1 : -1),
    longitude:
      ddMmSsToDecimal(match[6], match[7], match[8]) *
      (match[9] === "E" ? 1 : -1),
    symbol_table_id: match[5],
    symbol_code: match[10],
    course: null,
    speed: null,
    range: null,
    altitude: null, // in meters
    compressed: false,
    compression_byte: null,
    ...parsePositionDataExtension(data.substring(19)),
  };

  return coordinate_data;
}

function parseAprsMessage(data) {
  const pattern =
    /:([0-9A-Z\ \-]{9}):(ack|rej)?([^{\n]{0,67})(?:{([0-9a-zA-Z]{1,5}))?/;
  const match = data.match(pattern);
  if (!match) {
    log.warning("Invalid APRS message data:", data);
    return null;
  }

  const ack_or_rej = match[2] === "ack" || match[2] === "rej";

  const message_data = {
    addressee: match[1].replace(/ /g, ""),
    type: ack_or_rej ? `message-${match[2]}` : "message",
    text: ack_or_rej ? null : match[3],
    // message id gets stuck in 3 during ack/rej
    message_id: ack_or_rej ? match[3] : match[4] || null,
  };

  return message_data;
}

function parseAx25InfoField(info_field) {
  if (!info_field) {
    return null;
  }

  if (info_field.length < 2) {
    return null;
  }

  let packet_data = {
    type: null,
    timestamp: null,
    position: null,
    weather: null,
    message: null,
    raw_data: info_field,
  };

  const type_char = info_field.charAt(0);
  if (type_char === "!" || type_char === "=") {
    packet_data.type = "position-without-timestamp";
    packet_data.position = parsePosition(info_field.substring(1));
    packet_data.weather = parseWeatherData(info_field);
    if (packet_data.position.compressed) {
      packet_data.type = "compressed-position-without-timestamp";
    }
    if (packet_data.weather) {
      packet_data.type = "complete-weather-report-no-timestamp";
    }
  } else if (type_char === "/" || type_char === "@") {
    packet_data.type = "position-with-timestamp";
    packet_data.timestamp = parseTimestamp(info_field.substring(1));
    packet_data.position = parsePosition(info_field.substring(8));
    if (packet_data.position.compressed) {
      packet_data.type = "compressed-position-with-timestamp";
    }
    packet_data.weather = parseWeatherData(info_field);
    if (packet_data.weather) {
      packet_data.type = "complete-weather-report-with-timestamp";
    }
  } else if (type_char === ":") {
    // packet_data.type = "message";
    packet_data.message = parseAprsMessage(info_field);
    packet_data.type = packet_data.message.type;
  } else if (type_char === "[") {
    type = "mainhead-locator";
    console.error("Mainhead locator type not implemented yet");
  } else {
    return null;
  }

  return packet_data;
}

function processTNC2Data(data) {
  const regex =
    /^([0-9A-Z-]{3,9})>([0-9A-Z-]{1,9})((?:[,>][0-9A-Za-z-]{1,9}[*]?)*):(.{1,256})[\n\r]{0,2}$/g;
  const match = regex.exec(data);
  if (!match) {
    log.error("Failed to parse APRS-IS data:", data);
    return;
  }

  const source_callsign = match[1];
  const destination_callsign = match[2];
  const digi_path = match[3].replace(/^,/, ""); // Remove leading comma
  const information_field = match[4];

  // Parse the packet data
  let parsed = null;
  try {
    parsed = parseAx25InfoField(information_field);
  } catch (error) {
    log.error("Error processing TNC2 data:", error);
    return;
  }

  return {
    source_callsign: source_callsign,
    destination_callsign: destination_callsign,
    digi_path: digi_path,
    information_field: information_field,
    parsed: parsed,
    raw_data: data,
  };
}

module.exports = {
  processTNC2Data,
  parseAx25InfoField,
};
