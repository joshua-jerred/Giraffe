class Location {
  constructor() {
    this.valid = false;
    this.source = "n/d";

    this.timestamp = 0;
    this.latitude = 0;
    this.longitude = 0;
    this.altitude = 0;
  }

  json() {
    return {
      valid: this.valid,
      source: this.source,
      latitude: this.latitude,
      longitude: this.longitude,
      altitude: this.altitude,
    };
  }
}

module.exports = class LocationData {
  constructor(global_state) {
    this.global_state = global_state;

    this.current_location = {
      valid: false,
      source: "n/d",
      latitude: 0,
      longitude: 0,
      altitude: 0,
    };
  }
};
