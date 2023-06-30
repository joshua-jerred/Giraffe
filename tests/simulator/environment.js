const config = require("./sim_params.json").environment;

module.exports = class Environment {
  constructor() {
    this.state = "ground"; // ground, ascent, descent

    this.start = new Date();
    this.ascent_start = 0;
    this.descent_start = 0;

    this.elapsed = 0;

    this.alt = 0; // meters
    this.t = 0; // C
    this.p = 0; // KPa

    this.lat = 0.0;
    this.lon = 0.0;

    this.wind = {
      speed: 0,
      direction: 0,
    }

    this.wind_change_steps = 0;
    this.wind_change_step = 0;
    this.wind_dir_step = 0;
  }

  get env_data() {
    return {
      alt: this.alt,
      temp: this.t,
      pressure: this.p,
      wind: this.wind,
      lat: this.lat,
      lon: this.lon,
    }
  }

  begin(logging = false) {
    this.update();
    setInterval(() => {
      this.update(logging);
    }, 100);
  }

  update(logging) {
    // call every 0.1 seconds
    this.elapsed = Number((new Date() - this.start) / 1000, 2).toFixed(1);
    this.updateAltitude();
    this.calculatePandT();
    this.updateWind();
    this.updatePosition();

    if (logging) {
      this.log();
    }
  }

  log() {
    console.clear();
    let f_alt_m = Number(this.alt).toFixed(1);
    let f_alt_f = Number(this.alt * 3.28084).toFixed(1);

    let f_t_c = Number(this.t).toFixed(1);
    let f_t_f = Number(this.t * 9 / 5 + 32).toFixed(1);

    let f_p = Number(this.p * 10).toFixed(1);

    let f_ws_m_s = Number(this.wind.speed).toFixed(1);
    let f_ws_mph = Number(this.wind.speed * 2.23694).toFixed(1);
    
    let f_wdir = Number(this.wind.direction).toFixed(0);

    let f_lat = Number(this.lat).toFixed(5);
    let f_lon = Number(this.lon).toFixed(5);
    console.log(
      `
Time: ${this.elapsed} | State: ${this.state} | Altitude: ${f_alt_m}m/${f_alt_f}ft
Temperature: ${f_t_c}C/${f_t_f}F | Pressure: ${f_p}mbar
Wind: ${f_ws_m_s}ms/${f_ws_mph}mph from ${f_wdir}deg
Lat: ${f_lat} | Lon: ${f_lon}
      `
    );
  }

  launch() {
    this.state = "ascent";
    this.ascent_start = this.elapsed;
  }

  updateAltitude() {
    if (this.state == "ascent") {
      let ascent_time = this.elapsed - this.ascent_start;
      this.alt = config.ascent_rate_m_s * ascent_time;
    } else if (this.state == "descent") {
      this.alt -= config.descent_rate_m_s;
    }
  }

  updateWind() {
    let max = config.max_wind_speed_m_s;
    let min = config.min_wind_speed_m_s;
    let change_coef = config.wind_speed_change_coef;

    if (this.wind_change_steps <= 0) {
      this.wind_change_steps = Math.floor(Math.random() * 10 * 60); // max of 5 minutes
      this.wind_change_step = Math.random() * change_coef;
      this.wind_dir_step = Math.random() * config.wind_dir_change_coef;

      if (Math.random() > 0.5) {
        this.wind_change_step *= -1;
      }
      if (Math.random() > 0.5) {
        this.wind_dir_step *= -1;
      }
    }

    this.wind.speed += this.wind_change_step;
    this.wind.direction += this.wind_dir_step;

    this.wind_change_steps -= 1;
    if (this.wind.speed > max) {
      this.wind.speed = max;
    }
    if (this.wind.speed < min) {
      this.wind.speed = min;
    }

    if (this.wind.direction > 360) {
      this.wind.direction -= 360;
    }
    if (this.wind.direction < 0) {
      this.wind.direction += 360;
    }
  }

  calculatePandT() {
    if (this.alt > 25000) {
      // 25km and above
      this.t = -131.21 + 0.00299 * this.alt;
      this.p = 2.488 * Math.pow((this.t + 273.1) / 216.6, -11.388);
    } else if (this.alt > 11000) {
      // 11km to 25km
      this.t = -56.46;
      this.p = 22.65 * Math.exp(1.73 - 0.000157 * this.alt);
    } else if (this.alt > 0) {
      // 0 to 11km
      this.t = 15.04 - 0.00649 * this.alt;
      this.p = 101.29 * Math.pow((this.t + 273.1) / 288.08, 5.256);
    }
  }

  updatePosition() {
    this.lat += config.lat_step;
    this.lon += config.lon_step;
  }
}