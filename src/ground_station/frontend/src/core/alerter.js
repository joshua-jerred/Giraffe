/**
 * Alerter class for keeping track of alerts.
 */
module.exports = class Alerter {
  constructor() {
    this.alerts = {};
    this.suppressedAlertIds = [];

    // Load suppressed alerts from local storage
    try {
      let loaded = localStorage.getItem("suppressedAlerts");
      if (loaded !== null) {
        this.suppressedAlertIds = JSON.parse(loaded);
      }
      localStorage.setItem(
        "suppressedAlerts",
        JSON.stringify(this.suppressedAlertIds)
      );
    } catch (e) {
      console.log("Error loading suppressed alerts", e);
    }
  }

  suppressAlert(alert_id) {
    console.log("suppressed alert", alert_id);
    if (this.suppressedAlertIds.includes(alert_id)) {
      return; // Already suppressed
    }

    this.suppressedAlertIds.push(alert_id);
    localStorage.setItem(
      "suppressedAlerts",
      JSON.stringify(this.suppressedAlertIds)
    );
  }

  unSuppressAlert(alert_id) {
    console.log("unsuppressed alert", alert_id);
    if (!this.suppressedAlertIds.includes(alert_id)) {
      return; // Not suppressed
    }

    this.suppressedAlertIds = this.suppressedAlertIds.filter(
      (id) => id !== alert_id
    );
    localStorage.setItem(
      "suppressedAlerts",
      JSON.stringify(this.suppressedAlertIds)
    );
  }

  isAlertSuppressed(alert_id) {
    return this.suppressedAlertIds.includes(alert_id);
  }

  /**
   * Adds an alert to the list of alerts.
   * @param {string} alert_id - Unique identifier for the alert.
   * @param {string} alert_text - Text to display in the alert.
   * @param {int} alert_timeout - Time in seconds to display the alert. A time
   * of 0 will display the alert until it is manually removed.
   * @param {string} fix_link - Optional link to a page that will help the user
   * fix the problem.
   */
  addAlert(alert_id, alert_text, alert_timeout, fix_link = null) {
    this.alerts[alert_id] = {
      text: alert_text,
      timeout: alert_timeout,
      timestamp: Date.now(),
      fix_link: fix_link,
      suppressed: this.isAlertSuppressed(alert_id),
    };
  }

  /**
   * Removes an alert from the list of alerts. If the alert doesn't exist, it
   * does nothing.
   * @param {string} alert_id - Unique identifier for the alert.
   */
  clearAlert(alert_id) {
    if (this.alerts[alert_id] !== undefined) {
      delete this.alerts[alert_id];
    }
  }

  /**
   * Removes all alerts from the list of alerts.
   */
  clearAllAlerts() {
    this.alerts = {};
  }

  /**
   * @returns {Object} - Returns the list of alerts.
   */
  getAlerts(include_suppressed = false) {
    if (include_suppressed) {
      return this.alerts;
    }

    let alerts = {};
    for (let alert_id in this.alerts) {
      if (!this.suppressedAlertIds.includes(alert_id)) {
        alerts[alert_id] = this.alerts[alert_id];
      }
    }
    return alerts;
  }

  /**
   * @param {string} alert_id - Unique identifier for the alert.
   * @returns {Object} - Returns the alert with the given alert_id. If no alert
   * exists with the given alert_id, then returns null.
   */
  getAlert(alert_id) {
    if (this.alerts[alert_id] === undefined) {
      return null;
    }
    return this.alerts[alert_id];
  }

  /**
   * Dependant on the 'updateAlerts' method being called regularly.
   * @param {string} alert_id - Unique identifier for the alert.
   * @returns - Returns true if an alert with the given alert_id exists.
   */
  isAlertActive(alert_id) {
    if (this.alerts[alert_id] === undefined) {
      return false;
    }
    return true;
  }

  /// @brief Get the number of active alerts.
  /// @param include_suppressed If true, then suppressed alerts are included in
  /// the count.
  /// @return The number of active alerts.
  getNumAlerts(include_suppressed = false) {
    if (include_suppressed) {
      return Object.keys(this.alerts).length;
    }

    let numAlerts = 0;
    for (let alert_id in this.alerts) {
      if (!this.suppressedAlertIds.includes(alert_id)) {
        numAlerts++;
      }
    }
    // console.log("num alerts", numAlerts);
    return numAlerts;
  }

  /**
   * Goes through the list of alerts and removes any that have timed out.
   */
  updateAlerts() {
    for (let alert_id in this.alerts) {
      if (this.alerts[alert_id].timeout !== 0) {
        // console.log(
        // "time since raised",
        // Date.now() - this.alerts[alert_id].timestamp
        // );
        if (
          Date.now() - this.alerts[alert_id].timestamp >
          this.alerts[alert_id].timeout
        ) {
          this.clearAlert(alert_id);
        }
      }
    }
  }
};
