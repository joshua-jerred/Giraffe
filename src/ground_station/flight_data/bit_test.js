const gfs_bit_test_meta = require("../../../project/metadata/gfs_bit_test.json");
const GFS_BIT_TESTS = gfs_bit_test_meta.tests;
const GFS_TEST_STATUSES = gfs_bit_test_meta.statuses;

const validateAndRepairStatus = (status) => {
  if (status in GFS_TEST_STATUSES) {
    return status;
  }
  return "UNK";
};

module.exports = class BitTest {
  constructor(global_state) {
    // Initialize all groups and tests to "N/D" (No Data)
    this.bit_test_data = GFS_BIT_TESTS;
    for (const test_group_key in this.bit_test_data) {
      this.#setTestGroupStatus(test_group_key, "N/D");

      const tests_in_group_array = this.bit_test_data[test_group_key].tests;
      for (let i = 0; i < tests_in_group_array.length; i++) {
        const test_id = tests_in_group_array[i].id;
        this.#setTestStatus(test_group_key, test_id, "N/D");
      }
    }

    // Reference to the global state object. Access to the GFS TCP connection
    this.global_state = global_state;

    // True if a BIT test can be started/stopped/reset (Valid TCP Connection)
    this.bit_test_ready = false;

    // True if a BIT test is currently running
    this.bit_test_running = false;

    // The time the BIT test was started
    this.bit_test_start_time = null;

    // The overall status of the BIT test
    this.bit_test_status = "N/D";
  }

  processGetRequest(req, res) {
    res.json({
      tests: this.bit_test_data,
    });
  }

  processPostRequest(req, res) {
    const ACTION_PARAM_OPTIONS = ["start", "stop", "reset"];
    if (!("action" in req.query)) {
      res.status(400).json({
        error: "Missing  'action' in query: " + ACTION_PARAM_OPTIONS.join(", "),
      });
      return;
    } else if (!ACTION_PARAM_OPTIONS.includes(req.query.action)) {
      res.status(400).json({
        error: "Invalid 'action' in query: " + ACTION_PARAM_OPTIONS.join(", "),
      });
      return;
    }

    if (req.query.action === "start") {
      try {
        this.#startBitTest();
      } catch (error) {
        res.status(500).json({
          error: "Failed to start BIT test:" + error.message,
        });
        return;
      }
    } else if (req.query.action === "stop") {
      try {
        this.#stopBitTest();
      } catch (error) {
        res.status(500).json({
          error: "Failed to stop BIT test:" + error.message,
        });
        return;
      }
    } else if (req.query.action === "reset") {
      try {
        this.#resetBitTest();
      } catch (error) {
        res.status(500).json({
          error: "Failed to reset BIT test:" + error.message,
        });
        return;
      }
    }

    res.json({
      message: "success",
    });
  }

  #setTestGroupStatus(test_group, status) {
    status = validateAndRepairStatus(status);

    if (this.bit_test_data[test_group]) {
      this.bit_test_data[test_group].status = status;
    } else {
      console.log("Error: Invalid test group in BitTest.#setTestGroupStatus()");
    }
  }

  #setTestStatus(test_group, test_id, status) {
    status = validateAndRepairStatus(status);

    for (let i = 0; i < this.bit_test_data[test_group].tests.length; i++) {
      if (this.bit_test_data[test_group].tests[i].id === test_id) {
        this.bit_test_data[test_group].tests[i].status = status;
        return;
      }
    }

    console.log("Error: Invalid test ID in BitTest.#setTestStatus()");
  }

  #startBitTest() {}

  #stopBitTest() {}

  #resetBitTest() {}

  #processTestResults() {
    let total_tests = 0;
    let total_passed = 0;
    let total_failed = 0;
    let total_skipped = 0;
  }
};
