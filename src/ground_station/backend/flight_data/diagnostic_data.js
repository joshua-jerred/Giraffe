module.exports = class DiagnosticData {
  constructor(global_state) {
    this.global_state = global_state;
    this.num_total_errors = 0;
    this.num_active_errors = 0;
    this.active_errors = [];
  }

  updateFromTcp(error_frame) {
    const PROPERTIES = [
      "num_total_errors",
      "num_active_errors",
      "active_errors",
    ];
    for (let prop of PROPERTIES) {
      if (!error_frame.hasOwnProperty(prop)) {
        console.log("Error: Missing property in error frame: ", prop);
        return;
      }
    }

    this.num_total_errors = error_frame.num_total_errors;
    this.num_active_errors = error_frame.num_active_errors;
    this.active_errors = error_frame.active_errors;
  }

  getNumActiveErrors() {
    return this.num_active_errors;
  }

  getGeneralData() {
    return {
      num_total_errors: this.num_total_errors,
      num_active_errors: this.num_active_errors,
      active_errors: this.active_errors,
    };
  }
};
