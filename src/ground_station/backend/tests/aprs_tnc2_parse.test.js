const test = require("ava").serial;
const {
  processTNC2Data,
  parseAx25InfoField,
} = require("../aprs/aprs_tnc2_parse");

// test strings pulled form here:
// https://www.aprs.org/doc/APRS101.PDF

test("Parse APRS TNC2 Position Report - without Timestamp", (t) => {
  {
    // no timestamp, no APRS messaging, with comment
    const raw_packet = "!4903.50N/07201.75W-Test 001234";
    let result = parseAx25InfoField(raw_packet);

    t.truthy(result);
    t.is(result.type, "position-without-timestamp");
    t.is(result.timestamp, null);
    t.truthy(result.position);
    t.assert(result.position.latitude < 49.1);
    t.assert(result.position.latitude > 49);
    t.assert(result.position.longitude < -72);
    t.assert(result.position.longitude > -73);
    t.is(result.position.compressed, false);
    t.is(result.position.comment, "Test 001234");
  }
  {
    // no timestamp, no APRS messaging, altitude = 1234 ft.
    const raw_packet = "!4903.50N/07201.75W-Test /A=001234";
    let result = parseAx25InfoField(raw_packet);

    t.truthy(result);
    t.is(result.type, "position-without-timestamp");
    t.is(result.timestamp, null);
    t.truthy(result.position);
    t.is(result.position.altitude, 1234);
    t.is(result.position.comment, "Test /A=001234");
  }
  {
    // no timestamp, no APRS messaging, location to nearest degree.
    const raw_packet = "!49  .  N/072  .  W-";
    let result = parseAx25InfoField(raw_packet);

    t.truthy(result);
    t.is(result.type, "position-without-timestamp");
    t.is(result.timestamp, null);
    t.truthy(result.position);
    t.assert(result.position.latitude > 48.99);
    t.assert(result.position.latitude < 50.01);
    t.assert(result.position.longitude < -71.99);
    t.assert(result.position.longitude > -72.01);
    t.is(result.position.compressed, false);
    t.is(result.position.symbol_table_id, "/");
    t.is(result.position.symbol_code, "-");
    t.is(result.position.comment, "");
  }
});

test("Parse APRS TNC2 Position Report - with Timestamp", (t) => {
  {
    // with timestamp, no APRS messaging, zulu time, with comment.
    const raw_packet = "/092345z4903.50N/07201.75W>Test1234";
    let result = parseAx25InfoField(raw_packet);

    t.truthy(result);
    t.is(result.type, "position-with-timestamp");
    t.truthy(result.timestamp);
    t.is(result.timestamp.timezone, "zulu");
    t.is(result.timestamp.day, 9);
    t.is(result.timestamp.hour, 23);
    t.is(result.timestamp.minute, 45);
    t.is(result.timestamp.second, 0);
    t.truthy(result.position);
    t.assert(result.position.latitude < 49.1);
    t.assert(result.position.latitude > 49);
    t.assert(result.position.longitude < -72);
    t.assert(result.position.longitude > -73);
    t.is(result.position.compressed, false);
    t.is(result.position.comment, "Test1234");
    t.is(result.position.symbol_table_id, "/");
    t.is(result.position.symbol_code, ">");
  }
  {
    // with timestamp, with APRS messaging, local time, with comment.
    const raw_packet = "@092345/4903.50N/07201.75W>Test1234";
    let result = parseAx25InfoField(raw_packet);

    t.truthy(result);
    t.is(result.type, "position-with-timestamp");
    t.truthy(result.timestamp);
    t.is(result.timestamp.timezone, "local");
    t.is(result.timestamp.day, 9);
    t.is(result.timestamp.hour, 23);
    t.is(result.timestamp.minute, 45);
    t.is(result.timestamp.second, 0);
    t.truthy(result.position);
    t.assert(result.position.latitude < 49.1);
    t.assert(result.position.latitude > 49);
    t.assert(result.position.longitude < -72);
    t.assert(result.position.longitude > -73);
    t.is(result.position.compressed, false);
    t.is(result.position.comment, "Test1234");
    t.is(result.position.symbol_table_id, "/");
    t.is(result.position.symbol_code, ">");
    t.is(result.position.data_extension_type, "none");
    t.is(result.position.data_extension, null);
  }
});

test("Parse APRS TNC2 Position Report - with Data Extension - no Timestamp", (t) => {
  {
    // with timestamp, no APRS messaging, zulu time, with comment.
    const raw_packet = "=4903.50N/07201.75W#PHG5132";
    let result = parseAx25InfoField(raw_packet);

    t.truthy(result);
    t.is(result.type, "position-without-timestamp");
    t.truthy(result.position);
    t.assert(result.position.latitude < 49.1);
    t.assert(result.position.latitude > 49);
    t.assert(result.position.longitude < -72);
    t.assert(result.position.longitude > -73);
    t.is(result.position.compressed, false);
    t.is(result.position.comment, ""); // data extension, no comment
    t.is(result.position.symbol_table_id, "/");
    t.is(result.position.symbol_code, "#");
    t.is(result.position.data_extension_type, "power-height-gain");
    t.is(result.position.data_extension.phg, "5132");
  }
  {
    // weather report in the comment, *not* the bearing/speed data extension!
    const raw_packet =
      "=4903.50N/07201.75W_225/004g000t050r000p001h00b10138dU2k";
    let result = parseAx25InfoField(raw_packet);

    console.log("result", result);

    t.truthy(result);
    t.is(result.type, "complete-weather-report-no-timestamp");
    t.truthy(result.position);
    t.assert(result.position.latitude < 49.1);
    t.assert(result.position.latitude > 49);
    t.assert(result.position.longitude < -72);
    t.assert(result.position.longitude > -73);
    t.is(result.position.compressed, false);
    t.is(result.position.symbol_table_id, "/");
    t.is(result.position.symbol_code, "_");
    t.is(result.position.data_extension_type, "none"); // weather

    t.truthy(result.weather);
  }
});

test("Parse APRS TNC2 Weather Report", (t) => {
  {
    const expected_weather_data = {
      wind_direction: 225,
      wind_speed_mph: 4,
      gust_speed_mph: 12,
      temperature_f: 50,
      rainfall_1h: 1,
      rainfall_24h: 22,
      rainfall_since_midnight: null,
      humidity: 25,
      barometric_pressure: 1013.8,
    };

    // these are equivalent, the corse and speed are encoded differently
    const raw_packet_1 =
      "!4903.50N/07201.75W_220/004g005t077r000p000P000h50b09900wRSW";

    let result = parseAx25InfoField(raw_packet_1);

    t.truthy(result.weather);
    t.assert(result.type === "complete-weather-report-no-timestamp");
  }
  {
    const expected_weather_data = {
      wind_direction: 225,
      wind_speed_mph: 4,
      gust_speed_mph: 12,
      temperature_f: 50,
      rainfall_1h: 1,
      rainfall_24h: 22,
      rainfall_since_midnight: null,
      humidity: 25,
      barometric_pressure: 1013.8,
    };

    // these are equivalent, the corse and speed are encoded differently
    const raw_packet_1 =
      "=4903.50N/07201.75W_225/004g012t050r001p022h25b10138dU2k";
    const raw_packet_2 =
      "=4903.50N/07201.75W_c225s004g012t050r001p022h25b10138dU2k";

    let result_1 = parseAx25InfoField(raw_packet_1);
    let result_2 = parseAx25InfoField(raw_packet_2);

    t.truthy(result_1.weather);
    t.truthy(result_2.weather);

    for (const key in expected_weather_data) {
      t.is(result_1.weather[key], expected_weather_data[key], key);
      t.is(result_2.weather[key], expected_weather_data[key], key);
    }
  }
});

test("APRS Message Format", (t) => {
  {
    let raw_packet = ":WU2Z     :Testing out aprs regex";
    let result = parseAx25InfoField(raw_packet);

    t.truthy(result.message);
    t.is(result.type, "message");
    t.is(result.message.addressee, "WU2Z");
    t.is(result.message.text, "Testing out aprs regex");
    t.is(result.message.message_id, null);
  }
  {
    let raw_packet = ":WATT     :Testing hello{003";
    let result = parseAx25InfoField(raw_packet);

    t.truthy(result.message);
    t.assert(result.type === "message");
    t.is(result.message.addressee, "WATT");
    t.is(result.message.text, "Testing hello");
    t.is(result.message.message_id, "003");
  }
  {
    let raw_packet = ":KD9GDC-1 :test{m0007";
    let result = parseAx25InfoField(raw_packet);

    t.truthy(result.message);
    t.assert(result.type === "message");
    t.is(result.message.addressee, "KD9GDC-1");
    t.is(result.message.text, "test");
    t.is(result.message.message_id, "m0007");
  }
  {
    let raw_packet = ":KB2ICI-14:ack003";
    let result = parseAx25InfoField(raw_packet);

    t.truthy(result.message);
    t.assert(result.type === "message-ack");
    t.is(result.message.addressee, "KB2ICI-14");
    t.is(result.message.text, null);
    t.is(result.message.message_id, "003");
  }
  {
    let raw_packet = ":KB2ICI-14:rej050";
    let result = parseAx25InfoField(raw_packet);
    t.truthy(result.message);
    t.assert(result.type === "message-rej");
    t.is(result.message.addressee, "KB2ICI-14");
    t.is(result.message.text, null);
    t.is(result.message.message_id, "050");
  }
});
